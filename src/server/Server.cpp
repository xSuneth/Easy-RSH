#include "Server.h"
#include "CommandExecutor.h"
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
    std::cout << "Starting server on port " << port_ << "..." << std::endl;
    
    
    listen_socket_.create();   // Create socket
    
    listen_socket_.setReuseAddr(true);       // Set SO_REUSEADDR to avoid "Address already in use" errors
    
    listen_socket_.bind(port_);  // Bind to port
    
    listen_socket_.listen(5); // Listen for connections
    
    // Get all network interface IP addresses
    struct ifaddrs *ifaddr, *ifa;
    std::vector<std::string> ip_addresses;
    
    if (getifaddrs(&ifaddr) != -1) {
        for (ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
            if (ifa->ifa_addr == nullptr) continue;
            
            // Check for IPv4
            if (ifa->ifa_addr->sa_family == AF_INET) {
                void* addr = &((struct sockaddr_in*)ifa->ifa_addr)->sin_addr;
                char addressBuffer[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, addr, addressBuffer, INET_ADDRSTRLEN);
                
                std::string ip(addressBuffer);
                // Skip loopback
                if (ip != "127.0.0.1") {
                    ip_addresses.push_back(ip);
                }
            }
        }
        freeifaddrs(ifaddr);
    }
    
    // Display server information with clear formatting
    std::cout << "\n";
    std::cout << "========================================" << std::endl;
    std::cout << "    SERVER STARTED SUCCESSFULLY" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Port: " << port_ << std::endl;
    
    if (!ip_addresses.empty()) {
        std::cout << "\nAvailable on network:" << std::endl;
        for (const auto& ip : ip_addresses) {
            std::cout << "  • http://" << ip << ":" << port_ << std::endl;
        }
    }
    
    std::cout << "\nLocal connection:" << std::endl;
    std::cout << "  • http://localhost:" << port_ << std::endl;
    std::cout << "  • http://127.0.0.1:" << port_ << std::endl;
    
    std::cout << "\n----------------------------------------" << std::endl;
    std::cout << "To connect from another computer, run:" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    
    if (!ip_addresses.empty()) {
        std::cout << "  ./client " << ip_addresses[0] << " " << port_ << std::endl;
    } else {
        std::cout << "  ./client <server-ip> " << port_ << std::endl;
    }
    
    std::cout << "========================================" << std::endl;
    
    // Check firewall status and provide instructions
    std::cout << "\n⚠️  FIREWALL NOTICE:" << std::endl;
    std::cout << "If clients can't connect, allow port " << port_ << ":" << std::endl;
    std::cout << "  sudo ufw allow " << port_ << "/tcp" << std::endl;
    std::cout << "  sudo ufw status" << std::endl;
    std::cout << "\nOr with iptables:" << std::endl;
    std::cout << "  sudo iptables -A INPUT -p tcp --dport " << port_ << " -j ACCEPT" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "\nWaiting for connections..." << std::endl;
}

// Handle single client - echo mode
void Server::handleClientEcho(Socket& client_socket) {
    char buffer[BUFFER_SIZE];
    
    std::cout << "Client connected. Echo mode enabled." << std::endl;
    
    while (true) {
       
        std::memset(buffer, 0, BUFFER_SIZE);  // Clear buffer
        // Receive data from client
        ssize_t bytes_received = client_socket.recv(buffer, BUFFER_SIZE - 1, 0);
        
        if (bytes_received <= 0) {
            if (bytes_received == 0) {
                std::cout << "Client disconnected." << std::endl;
            } else {
                std::cerr << "Error receiving data." << std::endl;
            }
            break;
        }
        
        
        std::cout << "Received: " << buffer; // Print received 
      
        client_socket.send(buffer, bytes_received, 0);   // Echo back to client
    }
}

// Handle client - command execution mode 
void Server::handleClientCommand(Socket& client_socket) {
    char buffer[BUFFER_SIZE];
    
    std::cout << "Client connected. Command execution mode enabled." << std::endl;
    
    while (true) {
        // Clear buffer
        std::memset(buffer, 0, BUFFER_SIZE);
        
        // Receive command from client
        ssize_t bytes_received = client_socket.recv(buffer, BUFFER_SIZE - 1, 0);
        
        if (bytes_received <= 0) {
            if (bytes_received == 0) {
                std::cout << "Client disconnected." << std::endl;
            } else {
                std::cerr << "Error receiving data." << std::endl;
            }
            break;
        }
        
        std::string command(buffer, bytes_received);
        std::cout << "Executing command: " << command << std::flush;
        
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
            std::cout << "Waiting for client connection..." << std::endl;
            
            // Accept client connection
            Socket client_socket = listen_socket_.accept(client_addr);
            
            // Convert client address to string
            char client_ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
            std::cout << "Client connected from " << client_ip 
                      << ":" << ntohs(client_addr.sin_port) << std::endl;
            
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
                    std::cout << "Forked child process (PID: " << pid << ") to handle client" << std::endl;
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
            std::cerr << "Error: " << e.what() << std::endl;
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
    std::cout << "Server stopped." << std::endl;
}

// Enable or disable fork
void Server::setUseFork(bool use_fork) {
    use_fork_ = use_fork;
    if (use_fork) {
        std::cout << "Multi-client mode enabled (using fork)" << std::endl;
    } else {
        std::cout << "Single-client mode (no fork)" << std::endl;
    }
}

// Enable or disable command mode
void Server::setCommandMode(bool enable) {
    command_mode_ = enable;
    if (enable) {
        std::cout << "Command execution mode enabled" << std::endl;
    } else {
        std::cout << "Echo mode enabled" << std::endl;
    }
}
