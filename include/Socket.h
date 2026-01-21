#ifndef SOCKET_H
#define SOCKET_H

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>


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

    // Close the socket manually
    void close();
    
    // Reset with a new file descriptor
    void reset(int new_fd = -1);
    
    // Create a new TCP socket
    void create();
    
    // Bind socket to address and port
    void bind(int port);
    
    // Listen for incoming connections
    void listen(int backlog = 5);
    
    // Accept incoming connection (returns new Socket for client)
    Socket accept(sockaddr_in& client_addr);
    
    // Connect to server
    void connect(const std::string& host, int port);

    // Send data
    ssize_t send(const void* buffer, size_t length, int flags = 0);
    
    // Receive data
    ssize_t recv(void* buffer, size_t length, int flags = 0);
    
    // Set socket options
    void setReuseAddr(bool reuse);
    void setNonBlocking(bool nonblocking);
};

#endif // SOCKET_H