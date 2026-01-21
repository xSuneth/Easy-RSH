#ifndef SERVER_H
#define SERVER_H

#include "Socket.h"
#include <string>


class Server {
private:
    Socket listen_socket_;
    int port_;
    bool running_;
    bool use_fork_;  
    
    // Handle single client connection 
    void handleClientEcho(Socket& client_socket);
    
    // Handle client with command execution 
    void handleClientCommand(Socket& client_socket);
    
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
    
private:
    bool command_mode_;  // True - execute commands, False - echo
};

#endif 
