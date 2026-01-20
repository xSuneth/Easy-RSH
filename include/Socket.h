#ifndef SOCKET_H
#define SOCKET_H

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

/**
 * RAII wrapper for socket file descriptors
 * Ensures sockets are properly closed when going out of scope
 */
class Socket {
private:
    int fd_;  // Socket file descriptor
    
public:
    // Default constructor - creates invalid socket
    Socket();
    
    // Constructor with existing file descriptor
    explicit Socket(int fd);
    
    // Destructor - automatically closes socket
    ~Socket();
    
    // Delete copy constructor and copy assignment (prevent socket duplication)
    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;
    
    // Move constructor and move assignment
    Socket(Socket&& other) noexcept;
    Socket& operator=(Socket&& other) noexcept;
    
    // Get the raw file descriptor
    int get() const;
    
    // Check if socket is valid
    bool isValid() const;

// TODO