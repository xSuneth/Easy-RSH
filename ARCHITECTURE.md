# Architecture & Design Document

## System Architecture

### High-Level Overview

```
┌─────────────┐         ┌─────────────┐         ┌─────────────┐
│   Client 1  │◄───────►│             │         │             │
├─────────────┤         │             │         │   Command   │
│Interactive  │  TCP    │   Server    │  Pipe   │  Execution  │
│   Shell     │  Socket │  (Parent)   │◄───────►│ (Grandchild)│
│             │         │             │         │             │
│ - Login     │         │ - Auth      │         │ - execvp()  │
│ - Commands  │         │ - Fork      │         │ - Output    │
│ - Token     │         │ - Sessions  │         │   Capture   │
└─────────────┘         │             │         └─────────────┘
                        │             │
┌─────────────┐         │  Multi-     │         ┌─────────────┐
│   Client 2  │◄───────►│  Client     │         │    Data     │
└─────────────┘         │  Support    │         ├─────────────┤
                        │             │◄───────►│ users.txt   │
┌─────────────┐         │  SIGCHLD    │         │ (salt:hash) │
│   Client N  │◄───────►│  Handler    │         └─────────────┘
└─────────────┘         └─────────────┘
```

## Process Hierarchy

### Multi-Client Server with Command Execution

```
Server Main Process (PID: 1000)
│
├── SIGCHLD Handler (zombie reaping)
├── Listening Socket (port 8080)
│
├── Child Process (PID: 1001) [Client 1]
│   ├── Client Socket
│   ├── Command: "ls -la"
│   └── Grandchild (PID: 1002)
│       ├── execvp("ls", ["-la"])
│       ├── stdout → pipe[write]
│       └── stderr → pipe[write]
│
├── Child Process (PID: 1003) [Client 2]
│   ├── Client Socket
│   ├── Command: "pwd"
│   └── Grandchild (PID: 1004)
│       └── execvp("pwd", ...)
│
└── ... (more clients)
```

## Communication Flow

### Authentication Phase

```
Client                  Server                  Auth Module
  │                       │                          │
  ├──── connect() ───────►│                          │
  │◄─── accept() ─────────┤                          │
  │                       │                          │
  │◄─ "AUTH_REQUIRED" ────┤                          │
  │                       │                          │
  ├─ "username:pass" ────►│                          │
  │                       │                          │
  │                       ├── verifyUser() ─────────►│
  │                       │                          │
  │                       │   - Load salt from DB    │
  │                       │   - Hash salt+password   │
  │                       │   - Compare with stored  │
  │                       │                          │
  │                       │◄── true/false ───────────┤
  │                       │                          │
  │◄─ "AUTH_SUCCESS:      │                          │
  │     <token>" ─────────┤                          │
  │                       │                          │
```

### Command Execution Phase

```
Client                  Server Child            Auth
  │                       │                       │
  ├─ "CMD:ls -la"  ──────►│                       │
  │   + token              │                       │
  │                       │                       │
  │                       ├── validateToken() ───►│
  │                       │◄─── valid ────────────┤
  │                       │                       │
  │                       │ executeCommand()      │
  │                       │ (fork/exec/pipe)      │
  │                       │                       │
  │◄─ command output ─────┤                       │
  │                       │                       │
```

### Multi-Client Fork

```
Client 1         Server Parent       Child Process
  │                  │                     │
  ├─── connect() ───►│                     │
  │                  │                     │
  │                  ├──── fork() ────────►│
  │                  │                     │
  │                  │ close(client_fd)    │ close(listen_fd)
  │                  │                     │
  │◄─────────────────┼─────────────────────┤
  │                  │                     │
  │    Communication happens with child    │
  │◄────────────────────────────────────────┤
  │                  │                     │
  │                  │◄─── exit(0) ────────┤
  │                  │                     │
  │                  │ SIGCHLD received    │
  │                  │ waitpid() reaps     │
  │                  │ zombie              │
```

### Complete Command Execution with Pipe/Fork/Exec

```
Client          Server Child        Grandchild          Command
  │                 │                    │                  │
  ├─ "ls -la" ─────►│                    │                  │
  │                 │                    │                  │
  │                 │ create pipe[0,1]   │                  │
  │                 │                    │                  │
  │                 ├──── fork() ───────►│                  │
  │                 │                    │                  │
  │                 │ close(pipe[1])     │ close(pipe[0])   │
  │                 │                    │                  │
  │                 │                    │ dup2(pipe[1], 1) │
  │                 │                    │ dup2(pipe[1], 2) │
  │                 │                    │                  │
  │                 │                    ├─ execvp("ls") ──►│
  │                 │                    │                  │
  │                 │                    │                  │ execute
  │                 │                    │◄─── output ──────┤
  │                 │                    │                  │
  │                 │◄── pipe[0] ────────┤                  │
  │                 │ read output        │                  │
  │                 │                    │                  │
  │                 │ waitpid()          │                  │
  │                 │                    │◄─── exit(0) ─────┤
  │                 │                    │                  │
  │◄─ send result ──┤                    │                  │
  │                 │                    │                  │
```

## Class Design

### Auth Class

```cpp
class Auth {
public:
    static std::string hashPassword(const std::string& password, 
                                     const std::string& salt);
    static std::string generateSalt();
    static bool addUser(const std::string& username, 
                       const std::string& password);
    static bool verifyUser(const std::string& username, 
                          const std::string& password);
    static std::string generateToken();
    static bool validateToken(const std::string& token);
};
```

**Key Features:**

- SHA-256 hashing via OpenSSL
- Cryptographically secure random salt generation
- User database management (users.txt)
- Session token generation and validation
- No plaintext password storage

**Security Implementation:**

```
Stored Format: username:salt:hash
where hash = SHA256(salt + password)

Token Format: base64(random_bytes(32))
```

### Socket Class (RAII Pattern)

```cpp
class Socket {
    int fd_;  // File descriptor

    // Automatic cleanup via destructor
    ~Socket() { if (fd_ >= 0) close(fd_); }

    // Move semantics (no copying allowed)
    Socket(Socket&& other);
    Socket& operator=(Socket&& other);
};
```

**Key Features:**

- Automatic resource management (RAII)
- Move-only semantics prevent accidental fd duplication
- Wraps POSIX socket APIs with error handling

### Server Class

```cpp
class Server {
    Socket listen_socket_;
    bool use_fork_;
    bool command_mode_;

    void handleClientEcho(Socket&);
    void handleClientCommand(Socket&);
public:
    void run();  // Main server loop
};
```

**Responsibilities:**

- Accept client connections
- Fork child processes for concurrent clients
- Delegate to appropriate handler (echo vs command)

### Client Class

```cpp
class Client {
    Socket socket_;

    void runInteractiveShell();
    std::string sendCommand(const std::string&);
};
```

**Responsibilities:**

- Connect to server
- Provide interactive shell interface
- Send commands and display responses

### CommandExecutor Class

```cpp
class CommandExecutor {
    static Result execute(const std::string& cmd);
    static std::vector<std::string> parseCommand();
    static std::string readFromPipe(int fd);
};
```

**Responsibilities:**

- Parse command string into argv array
- Create pipe for IPC
- Fork grandchild process
- Execute command via execvp
- Capture and return output

## OS Concepts Implementation

### 1. Process Management

**fork() - Process Creation:**

- Server forks child per client (Phase 2)
- Child forks grandchild for command execution (Phase 3)

**execvp() - Program Execution:**

- Grandchild replaces itself with requested command
- Preserves environment variables

**wait() / waitpid() - Process Synchronization:**

- Child waits for grandchild to complete
- Parent uses SIGCHLD + waitpid(WNOHANG) for zombie reaping

### 2. Inter-Process Communication

**Pipe:**

```c
int pipefd[2];
pipe(pipefd);  // pipefd[0] = read, pipefd[1] = write

// Grandchild writes output
dup2(pipefd[1], STDOUT_FILENO);
execvp(...);

// Parent reads output
read(pipefd[0], buffer, size);
```

### 3. Signal Handling

**SIGCHLD Handler:**

```c
void sigchldHandler(int sig) {
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

struct sigaction sa;
sa.sa_handler = sigchldHandler;
sigaction(SIGCHLD, &sa, NULL);
```

**Purpose:** Prevent zombie processes when child processes exit

### 4. Network Programming

**Socket API Usage:**

```c
// Server
socket() → bind() → listen() → accept()

// Client
socket() → connect()

// Both
send() / recv()
```

## File Descriptors Management

### Per Process

- **Server Parent:** Listening socket (fd 3)
- **Server Child:** Client socket (fd 4), pipe read end (fd 5)
- **Grandchild:** Client socket (closed), pipe write end (dup2 → stdout/stderr)

### Resource Cleanup

- Parent closes client socket after fork
- Child closes listening socket after fork
- Grandchild closes unused pipe ends
- RAII ensures automatic cleanup on exceptions

## Error Handling Strategy

### Levels of Error Handling

**1. System Call Errors:**

- Check return values
- Use strerror(errno) for messages
- Throw exceptions for unrecoverable errors

**2. Network Errors:**

- EINTR (interrupted system call) → retry with SA_RESTART
- EPIPE (broken pipe) → client disconnected
- Connection timeout → graceful close

**3. Process Errors:**

- Fork failure → log error, continue accepting
- Exec failure → exit child with code 127
- Zombie accumulation → SIGCHLD handler

**4. Application Errors:**

- Empty commands → return error message
- Command not found → captured by execvp failure
- Buffer overflow → size limits enforced

## Concurrency Model

### Fork-Per-Client (Process-Based)

- Each client handled by separate process
- Complete address space isolation
- No shared memory issues
- Parent continues accepting new clients

**Advantages:**

- Simple to implement
- No synchronization needed
- Crashes isolated to one client

**Disadvantages:**

- Higher memory overhead than threads
- Process creation cost
- No shared state between clients

## Security Considerations

### Implemented Security Features

1. ✅ **User Authentication**
   - SHA-256 password hashing
   - Random salt per user (32 bytes)
   - No plaintext password storage
   - Session token-based authorization

2. ✅ **Secure Password Storage**
   - Format: `username:salt:hash`
   - Salt prevents rainbow table attacks
   - Hash prevents password recovery

3. ✅ **Session Management**
   - Unique tokens per login session
   - Token required for command execution
   - Cryptographically random token generation

### Remaining Limitations

1. ⚠️ No encryption (plaintext communication over TCP)
2. ⚠️ No command whitelisting or sandboxing
3. ⚠️ Runs with server's user privileges
4. ⚠️ No rate limiting or brute-force protection
5. ⚠️ Tokens stored in memory only (lost on restart)
6. ⚠️ No audit logging of executed commands

### Future Security Improvements

1. **TLS/SSL Encryption:** Encrypt all network traffic
2. **Command Whitelist:** Restrict allowed commands
3. **Privilege Separation:** Drop privileges after authentication
4. **Rate Limiting:** Prevent brute-force attacks
5. **Persistent Sessions:** Store tokens in secure database
6. **Audit Trail:** Log all authentication attempts and commands
7. **Argon2/bcrypt:** Use stronger password hashing algorithms
8. **Two-Factor Authentication:** Add TOTP support

## Performance Characteristics

### Resource Usage

- **Memory:** ~4-8 KB per socket buffer
- **Processes:** 1 parent + N children (one per client)
- **File Descriptors:** 3 per client (socket + pipe ends)

### Scalability

- Limited by system's process/fd limits
- Typical Linux: 1000-4000 processes
- For higher scale: consider thread pool or event-driven model

## Testing Strategy

### Unit Testing (Conceptual)

- Socket creation/binding
- Command parsing
- Pipe creation and reading

### Integration Testing

- Client-server communication
- Command execution and output capture
- Concurrent client handling

### System Testing

- Multiple simultaneous clients
- Long-running connections
- Error scenarios (network failures, invalid commands)

## Future Enhancements

1. **Thread Pool Model:** Replace fork with thread pool for better scalability
2. **Event-Driven I/O:** Use epoll/kqueue for high-concurrency
3. **Persistent Connections:** Keep connections alive across multiple commands
4. **Command History:** Track executed commands
5. **Authentication:** Add user login system
6. **Encryption:** SSL/TLS support
7. **Shell Features:** Support pipes, redirects, wildcards
8. **Resource Limits:** CPU/memory limits per command

## References

### POSIX APIs Used

- Socket: `socket()`, `bind()`, `listen()`, `accept()`, `connect()`
- Process: `fork()`, `execvp()`, `wait()`, `waitpid()`, `exit()`
- IPC: `pipe()`, `dup2()`, `read()`, `write()`
- Signal: `signal()`, `sigaction()`
- File: `open()`, `close()`, `fcntl()`

### Standards Compliance

- POSIX.1-2001
- C++17
- TCP/IP (RFC 793)
