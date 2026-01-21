#include "Client.h"
#include <iostream>
#include <cstring>

constexpr size_t BUFFER_SIZE = 4096;


Client::Client(const std::string& host, int port)
    : server_host_(host), server_port_(port), connected_(false) {
}

// Connect to server
bool Client::connect() {
    try {
        std::cout << "Connecting to " << server_host_ << ":" << server_port_ << "..." << std::endl;
        
        // Create socket
        socket_.create();
        
        // Connect to server
        socket_.connect(server_host_, server_port_);
        
        connected_ = true;
        std::cout << "Connected successfully!" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Connection failed: " << e.what() << std::endl;
        connected_ = false;
        return false;
    }
}

/
void Client::disconnect() {
    if (connected_) {
        socket_.close();
        connected_ = false;
        std::cout << "Disconnected from server." << std::endl;
    }
}


std::string Client::sendCommand(const std::string& command) {
    if (!connected_) {
        throw std::runtime_error("Not connected to server");
    }
    
   
    socket_.send(command.c_str(), command.length(), 0);
    
    
    char buffer[BUFFER_SIZE];
    std::memset(buffer, 0, BUFFER_SIZE);
    
    ssize_t bytes_received = socket_.recv(buffer, BUFFER_SIZE - 1, 0);
    
    if (bytes_received <= 0) {
        if (bytes_received == 0) {
            std::cout << "Server closed connection." << std::endl;
            connected_ = false;
        }
        return "";
    }
    
    return std::string(buffer, bytes_received);
}


void Client::runInteractiveShell() {
    if (!connected_) {
        std::cerr << "Not connected to server. Call connect() first." << std::endl;
        return;
    }
    
    std::cout << "\n=== Remote Command Shell ===" << std::endl;
    std::cout << "Type commands to execute on remote server." << std::endl;
    std::cout << "Type 'exit' or 'quit' to disconnect.\n" << std::endl;
    
    std::string input;
    
    while (connected_) {
        
        std::cout << "remote> " << std::flush;
        
        
        if (!std::getline(std::cin, input)) {
           
            break;
        }
        
      
        size_t start = input.find_first_not_of(" \t\n\r");
        size_t end = input.find_last_not_of(" \t\n\r");
        
        if (start == std::string::npos) {
            
            continue;
        }
        
        input = input.substr(start, end - start + 1);
        
    
        if (input == "exit" || input == "quit") {
            std::cout << "Disconnecting..." << std::endl;
            break;
        }
     
        if (input.empty()) {
            continue;
        }
        
        try {
            
            std::string command = input + "\n";
            
           
            std::string response = sendCommand(command);
            
            if (!response.empty()) {
                std::cout << response;
                if (response.back() != '\n') {
                    std::cout << std::endl;
                }
            }
            
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            break;
        }
    }
    
    disconnect();
}


bool Client::isConnected() const {
    return connected_;
}
