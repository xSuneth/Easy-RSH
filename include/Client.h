#ifndef CLIENT_H
#define CLIENT_H

#include "Socket.h"
#include <string>

/**
 * Client class for connecting to remote command server
 */

class Client {
private:
    Socket socket_;
    std::string server_host_;
    int server_port_;
    bool connected_;
    
public:
    
    Client(const std::string& host, int port);  // Constructor
    
    bool connect();   // Connect to server
    
    void disconnect();    // Disconnect from server
    
    std::string sendCommand(const std::string& command);    // Send a command to server and receive response
    
    void runInteractiveShell();    // Run interactive shell
    
    bool isConnected() const;      // Check if connected
};

#endif // CLIENT_H
