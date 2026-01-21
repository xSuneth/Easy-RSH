#include "Server.h"
#include "CommandExecutor.h"
#include "Colors.h"
#include <iostream>
#include <cstring>
#include <sys/wait.h>
#include <signal.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>

constexpr size_t BUFFER_SIZE = 4096;

// Signal handler for SIGCHLD to reap zombie processes
void sigchldHandler(int sig) {
    (void)sig;  
    
    // Use WNOHANG to make waitpid non-blocking
    while (waitpid(-1, nullptr, WNOHANG) > 0) {
        // Keep reaping until no more dead children
    }
}

Server::Server(int port) 
    : port_(port), running_(false), use_fork_(false), command_mode_(false) {
}

// Start server
void Server::start() {
    listen_socket_.create();
    listen_socket_.setReuseAddr(true);
    listen_socket_.bind(port_);
    listen_socket_.listen(5);
    
    // Get network IP
    struct ifaddrs *ifaddr, *ifa;
    std::vector<std::string> ip_addresses;
    
    if (getifaddrs(&ifaddr) != -1) {
        for (ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
            if (ifa->ifa_addr == nullptr) continue;
            if (ifa->ifa_addr->sa_family == AF_INET) {
                void* addr = &((struct sockaddr_in*)ifa->ifa_addr)->sin_addr;
                char addressBuffer[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, addr, addressBuffer, INET_ADDRSTRLEN);
                std::string ip(addressBuffer);
                if (ip != "127.0.0.1") {
                    ip_addresses.push_back(ip);
                }
            }
        }
        freeifaddrs(ifaddr);
    }
    
    // Simple server info
    std::cout << Color::PURPLE << "Server listening on port " << Color::BG_PURPLE << " " << port_ << " " << Color::RESET << std::endl;
    
    if (!ip_addresses.empty()) {
        std::cout << Color::GRAY << "Network: " << ip_addresses[0] << ":" << port_ << Color::RESET << std::endl;
    }
    std::cout << Color::GRAY << "Local:   127.0.0.1:" << port_ << Color::RESET << std::endl;
    
    if (!ip_addresses.empty()) {
        std::cout << Color::GRAY << "\nConnect: ./client " << ip_addresses[0] << " " << port_ << Color::RESET << std::endl;
    }
    std::cout << std::endl;
}

// Handle single client - echo mode
void Server::handleClientEcho(Socket& client_socket) {
    char buffer[BUFFER_SIZE];
    
    std::cout << Color::GRAY << "Client connected (Echo mode)" << Color::RESET << std::endl;
    
    while (true) {
       
        std::memset(buffer, 0, BUFFER_SIZE);  // Clear buffer
        // Receive data from client
        ssize_t bytes_received = client_socket.recv(buffer, BUFFER_SIZE - 1, 0);
        
        if (bytes_received <= 0) {
            if (bytes_received == 0) {
                std::cout << Color::GRAY << "Client disconnected" << Color::RESET << std::endl;
            } else {
                std::cerr << Color::ROSE << "Error receiving data" << Color::RESET << std::endl;
            }
            break;
        }
        
        
        std::cout << Color::GRAY << "Received: " << Color::RESET << buffer; // Print received 
      
        client_socket.send(buffer, bytes_received, 0);   // Echo back to client
    }
}

// Handle client - command execution mode 
void Server::handleClientCommand(Socket& client_socket) {
    char buffer[BUFFER_SIZE];
    
    std::cout << Color::GRAY << "Client connected (Command mode)" << Color::RESET << std::endl;
    
    while (true) {
        // Clear buffer
        std::memset(buffer, 0, BUFFER_SIZE);
        
        // Receive command from client
        ssize_t bytes_received = client_socket.recv(buffer, BUFFER_SIZE - 1, 0);
        
        if (bytes_received <= 0) {
            if (bytes_received == 0) {
                std::cout << Color::GRAY << "Client disconnected" << Color::RESET << std::endl;
            } else {
                std::cerr << Color::ROSE << "Error receiving data" << Color::RESET << std::endl;
            }
            break;
        }
        
        std::string command(buffer, bytes_received);
        std::cout << Color::GRAY << "Executing: " << Color::BG_PURPLE << " " << command.substr(0, command.length()-1) << " " << Color::RESET << std::endl;
        
        // Execute command
        CommandExecutor::Result result = CommandExecutor::execute(command);
        
        // Prepare response
        std::string response;
        if (!result.output.empty()) {
            response = result.output;
        } else {
            response = "(no output)\n";
        }
        
        // Add exit code if command failed
        if (!result.success && result.exit_code >= 0) {
            response += "[Exit code: " + std::to_string(result.exit_code) + "]\n";
        }
        
        // Send response back to client
        try {
            client_socket.send(response.c_str(), response.length(), 0);
        } catch (const std::exception& e) {
            std::cerr << "Error sending response: " << e.what() << std::endl;
            break;
        }
    }
}

// Main server -  loop
void Server::run() {
    running_ = true;
    
    // Install SIGCHLD handler if using fork
    if (use_fork_) {
        struct sigaction sa;
        sa.sa_handler = sigchldHandler;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = SA_RESTART;  // Restart interrupted system calls
        
        if (sigaction(SIGCHLD, &sa, nullptr) < 0) {
            std::cerr << "Warning: Failed to install SIGCHLD handler: " 
                      << strerror(errno) << std::endl;
        } else {
            std::cout << "SIGCHLD handler installed for zombie reaping" << std::endl;
        }
    }
    
    while (running_) {
        try {
            sockaddr_in client_addr;
            std::cout << Color::DIM << "Waiting for connection..." << Color::RESET << std::endl;
            
            // Accept client connection
            Socket client_socket = listen_socket_.accept(client_addr);
            
            // Convert client address to string
            char client_ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
            std::cout << Color::PURPLE << "Connection from " << Color::BG_MINT << " " << client_ip 
                      << ":" << ntohs(client_addr.sin_port) << " " << Color::RESET << std::endl;
            
            if (use_fork_) {
                // Fork to handle client in separate process (Phase 2)
                pid_t pid = fork();
                
                if (pid < 0) {
                    std::cerr << "Fork failed: " << strerror(errno) << std::endl;
                    // Continue accepting other clients
                    continue;
                }
                
                if (pid == 0) {
                    // Child process
                    listen_socket_.close();  // Child doesn't need listening socket
                    
                    try {
                        if (command_mode_) {
                            handleClientCommand(client_socket);
                        } else {
                            handleClientEcho(client_socket);
                        }
                    } catch (const std::exception& e) {
                        std::cerr << "Error in child process: " << e.what() << std::endl;
                    }
                    exit(0);// Exit child process
                } else {
                    
                    // Parent process
                    client_socket.close();  // Parent doesn't need client socket
                    std::cout << Color::GRAY << "Spawned process (PID: " << pid << ")" << Color::RESET << std::endl;
                }
            } else {
                // Handle client in main process (Phase 1)
                if (command_mode_) {
                    handleClientCommand(client_socket);
                } else {
                    handleClientEcho(client_socket);
                }
            }
            
        } catch (const std::exception& e) {
            std::cerr << Color::ROSE << "Error: " << e.what() << Color::RESET << std::endl;
            if (!use_fork_) {
                // continue to accept new connections in single-client mode
                continue;
            }
        }
    }
}

// Stop server
void Server::stop() {
    running_ = false;
    listen_socket_.close();
    std::cout << Color::GRAY << "\nServer stopped." << Color::RESET << std::endl;
}

// Enable or disable fork
void Server::setUseFork(bool use_fork) {
    use_fork_ = use_fork;
    if (use_fork) {
        std::cout << Color::GRAY << "Mode: Multi-client (fork)" << Color::RESET << std::endl;
    } else {
        std::cout << Color::GRAY << "Mode: Single-client" << Color::RESET << std::endl;
    }
}

// Enable or disable command mode
void Server::setCommandMode(bool enable) {
    command_mode_ = enable;
    if (enable) {
        std::cout << Color::GRAY << "Mode: Command execution" << Color::RESET << std::endl;
    } else {
        std::cout << Color::GRAY << "Mode: Echo" << Color::RESET << std::endl;
    }
}
