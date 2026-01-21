#include "Socket.h"
#include <stdexcept>
#include <cstring>
#include <fcntl.h>

// Default constructor
Socket::Socket() : fd_(-1) {}

// Constructor with existing file descriptor
Socket::Socket(int fd) : fd_(fd) {}

// Destructor
Socket::~Socket() {
    close();
}

// Move constructor
Socket::Socket(Socket&& other) noexcept : fd_(other.fd_) {
    other.fd_ = -1;
}

// Move assignment
Socket& Socket::operator=(Socket&& other) noexcept {
    if (this != &other) {
        close();
        fd_ = other.fd_;
        other.fd_ = -1;
    }
    return *this;
}

// Get file descriptor
int Socket::get() const {
    return fd_;
}

// Check validity
bool Socket::isValid() const {
    return fd_ >= 0;
}

// Close socket
void Socket::close() {
    if (fd_ >= 0) {
        ::close(fd_);
        fd_ = -1;
    }
}

// Reset with new file descriptor
void Socket::reset(int new_fd) {
    close();
    fd_ = new_fd;
}

// Create new TCP socket
void Socket::create() {
    fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (fd_ < 0) {
        throw std::runtime_error(std::string("Failed to create socket: ") + strerror(errno));
    }
}

// Bind to port
void Socket::bind(int port) {
    if (!isValid()) {
        throw std::runtime_error("Cannot bind invalid socket");
    }
    
    sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    
    if (::bind(fd_, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        throw std::runtime_error(std::string("Failed to bind socket: ") + strerror(errno));
    }
}

// Listen for connections
void Socket::listen(int backlog) {
    if (!isValid()) {
        throw std::runtime_error("Cannot listen on invalid socket");
    }
    
    if (::listen(fd_, backlog) < 0) {
        throw std::runtime_error(std::string("Failed to listen: ") + strerror(errno));
    }
}

// Accept connection
Socket Socket::accept(sockaddr_in& client_addr) {
    if (!isValid()) {
        throw std::runtime_error("Cannot accept on invalid socket");
    }
    
    socklen_t addr_len = sizeof(client_addr);
    int client_fd = ::accept(fd_, (struct sockaddr*)&client_addr, &addr_len);
    
    if (client_fd < 0) {
        throw std::runtime_error(std::string("Failed to accept connection: ") + strerror(errno));
    }
    
    return Socket(client_fd);
}

// Connect to server
void Socket::connect(const std::string& host, int port) {
    if (!isValid()) {
        throw std::runtime_error("Cannot connect with invalid socket");
    }
    
    sockaddr_in server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    
    // Convert host string to network address
    if (inet_pton(AF_INET, host.c_str(), &server_addr.sin_addr) <= 0) {
        throw std::runtime_error("Invalid address: " + host);
    }
    
    if (::connect(fd_, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        throw std::runtime_error(std::string("Failed to connect: ") + strerror(errno));
    }
}

// Send data
ssize_t Socket::send(const void* buffer, size_t length, int flags) {
    if (!isValid()) {
        throw std::runtime_error("Cannot send on invalid socket");
    }
    
    ssize_t sent = ::send(fd_, buffer, length, flags);
    if (sent < 0) {
        throw std::runtime_error(std::string("Failed to send: ") + strerror(errno));
    }
    
    return sent;
}

// Receive data
ssize_t Socket::recv(void* buffer, size_t length, int flags) {
    if (!isValid()) {
        throw std::runtime_error("Cannot receive on invalid socket");
    }
    
    ssize_t received = ::recv(fd_, buffer, length, flags);
    if (received < 0) {
        throw std::runtime_error(std::string("Failed to receive: ") + strerror(errno));
    }
    
    return received;
}

// Set SO_REUSEADDR option
void Socket::setReuseAddr(bool reuse) {
    if (!isValid()) {
        throw std::runtime_error("Cannot set option on invalid socket");
    }
    
    int opt = reuse ? 1 : 0;
    if (setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        throw std::runtime_error(std::string("Failed to set SO_REUSEADDR: ") + strerror(errno));
    }
}

// Set non-blocking mode
void Socket::setNonBlocking(bool nonblocking) {
    if (!isValid()) {
        throw std::runtime_error("Cannot set option on invalid socket");
    }
    
    int flags = fcntl(fd_, F_GETFL, 0);
    if (flags < 0) {
        throw std::runtime_error(std::string("Failed to get socket flags: ") + strerror(errno));
    }
    
    if (nonblocking) {
        flags |= O_NONBLOCK;
    } else {
        flags &= ~O_NONBLOCK;
    }
    
    if (fcntl(fd_, F_SETFL, flags) < 0) {
        throw std::runtime_error(std::string("Failed to set socket flags: ") + strerror(errno));
    }
}
