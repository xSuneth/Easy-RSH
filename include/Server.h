#ifndef SERVER_H
#define SERVER_H

#include "Socket.h"
#include "Auth.h"
#include <string>
#include <memory>


class Server {
private:
    Socket listen_socket_;
    int port_;
    bool running_;
    bool use_fork_;  
    std::shared_ptr<Auth> auth_;  // Authentication module
    bool require_auth_;           // Whether authentication is required
    std::string current_dir_;  // Track current working directory
    bool restart_requested_;      // Flag to request server restart

    
    // Handle single client connection 
    void handleClientEcho(Socket& client_socket);
    
    // Handle client with command execution 
    void handleClientCommand(Socket& client_socket);
    
    // Authenticate a client
    std::string authenticateClient(Socket& client_socket);
    
    // Handle cd command
    std::string handleCdCommand(const std::string& path);

    
public:
    explicit Server(int port);
    
    // Start the server 
    void start();
    
    // Main server loop
    void run();
    
    // Stop the server
    void stop();
    
    // Enable/disable fork 
    void setUseFork(bool use_fork);
    
    // Enable/disable command execution mode
    void setCommandMode(bool enable);
    
    // Enable/disable authentication
    void setRequireAuth(bool require);
    
    // Get authentication module
    std::shared_ptr<Auth> getAuth();
    
    // Check if restart was requested
    bool isRestartRequested() const;
    
private:
    bool command_mode_;  // True - execute commands, False - echo
};

#endif 
