#include "Client.h"
#include "Colors.h"
#include <iostream>
#include <cstring>
#include <sstream>

constexpr size_t BUFFER_SIZE = 4096;


Client::Client(const std::string& host, int port)
    : server_host_(host), server_port_(port), connected_(false) {
}

// Connect to server
bool Client::connect() {
    try {
        std::cout << Color::GRAY << "Connecting to " << server_host_ << ":" << server_port_ << "..." << Color::RESET << std::endl;
        
        // Create socket
        socket_.create();
        
        // Connect to server
        socket_.connect(server_host_, server_port_);
        
        connected_ = true;
        std::cout << Color::PURPLE << "Connected to " << Color::BG_PURPLE << " " << server_host_ << ":" << server_port_ << " " << Color::RESET << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << Color::ROSE << "Connection failed: " << e.what() << Color::RESET << std::endl;
        connected_ = false;
        return false;
    }
}


void Client::disconnect() {
    if (connected_) {
        socket_.close();
        connected_ = false;
        std::cout << Color::GRAY << "\nDisconnected from server." << Color::RESET << std::endl;
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
        std::cerr << Color::ROSE << "Not connected to server. Call connect() first." << Color::RESET << std::endl;
        return;
    }
    
    std::cout << "\n" << Color::GRAY << "Type 'exit' or 'quit' to disconnect." << Color::RESET << "\n" << std::endl;
    
    std::string input;
    
    while (connected_) {
        
        std::cout << Color::PURPLE << Color::BOLD << "remote" << Color::RESET << Color::GRAY << "> " << Color::RESET << std::flush;
        
        
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
            std::cout << Color::GRAY << "Disconnecting..." << Color::RESET << std::endl;
            break;
        }
     
        if (input.empty()) {
            continue;
        }
        
        try {
            
            std::string command = input + "\n";
            
           
            std::string response = sendCommand(command);
            
            if (!response.empty()) {
                // Add left margin to output for visual separation
                std::string line;
                std::istringstream stream(response);
                while (std::getline(stream, line)) {
                    std::cout << Color::GRAY << "  │ " << Color::RESET << line << std::endl;
                }
            }
            
        } catch (const std::exception& e) {
            std::cerr << Color::ROSE << "Error: " << e.what() << Color::RESET << std::endl;
            break;
        }
    }
    
    disconnect();
}


bool Client::isConnected() const {
    return connected_;
}
