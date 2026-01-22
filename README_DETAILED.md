# README_DETAILED.md
## Remote Command Execution System – Detailed Documentation

This document provides deeper architectural + implementation details for the system.

---

## 1) System Overview

This project is a TCP-based client–server system that supports:
- Secure login using SHA-256 + salt (OpenSSL)
- Remote command execution on the server machine
- Returning command output back to the client
- Handling `cd` as a special server-side action

---

## 2) Main Components

### 2.1 Socket Layer (`Socket.h / Socket.cpp`)
Responsible for:
- Creating/closing socket file descriptors safely
- Wrapper around low-level `socket()`, `close()`, etc.
- Resource management (RAII style)

### 2.2 Server (`Server.h / Server.cpp`)
Responsible for:
- Listening for incoming client connections
- Accepting client sockets
- Handling each client connection
- Connecting authentication + command execution modules

### 2.3 Client (`Client.h / Client.cpp`)
Responsible for:
- Connecting to server IP/Port
- Sending login credentials
- Sending commands and receiving outputs

### 2.4 Authentication (`Auth.h / Auth.cpp`)
Responsible for:
- Loading user database `data/users.txt`
- Validating login credentials
- Generating session tokens
- Salted SHA-256 hashing using OpenSSL

User format:
```
username:salt:hash
```

### 2.5 Command Execution (`CommandExecutor.h / CommandExecutor.cpp`)
Responsible for:
- Running commands on server OS
- Capturing output
- Sending output back to client
- Special handling for directory change via `cd`

---

## 3) Authentication Logic

### 3.1 Why Salt?
Salt prevents two identical passwords from producing the same hash.
This increases protection against precomputed rainbow tables.

### 3.2 Hash Process (Conceptual)
```
hash = SHA256(salt + password)
```

Then stored in `users.txt`.

### 3.3 Session Token
After success:
- A token is issued for the session
- Server uses it to keep track of authenticated state

---

## 4) Remote Command Execution

Typical flow:
1. Client sends a command string
2. Server validates authentication state
3. Server executes command
4. Server sends output back

### 4.1 `cd` Special Handling
`cd` is not a standalone executable like `ls`.
It must be handled by the server process itself using:
- `chdir(path)`
- `getcwd()` for confirmation

---

## 5) Build System

Uses `Makefile` targets:
- `server`
- `client`
- `adduser`
- `clean`
- `all`

OpenSSL libs required:
- `-lssl`
- `-lcrypto`

Threading may require:
- `-lpthread`

---

## 6) Suggested Enhancements (If Extending)
- Add TLS encrypted sockets (OpenSSL TLS layer)
- Add command allowlist to block dangerous commands
- Log every executed command (audit trail)
- Add rate-limit / lockout on repeated wrong passwords
- Replace SHA-256 password hashing with Argon2/bcrypt

---

## 7) Credits / Authors
Team 1 – Operating Systems Coursework
