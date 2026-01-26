# Remote Command Execution System (Client–Server) with OpenSSL Authentication

```
 ███████╗ █████╗ ███████╗██╗   ██╗    ██████╗ ███████╗██╗  ██╗
 ██╔════╝██╔══██╗██╔════╝╚██╗ ██╔╝    ██╔══██╗██╔════╝██║  ██║
 █████╗  ███████║███████╗ ╚████╔╝     ██████╔╝███████╗███████║
 ██╔══╝  ██╔══██║╚════██║  ╚██╔╝      ██╔══██╗╚════██║██╔══██║
 ███████╗██║  ██║███████║   ██║       ██║  ██║███████║██║  ██║
 ╚══════╝╚═╝  ╚═╝╚══════╝   ╚═╝       ╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝
                    Easy Remote Shell Server v1.0
```
 

A C++ TCP **client–server command execution system** that allows a remote client to connect to a server and execute shell commands securely using **SHA-256 + Salt password authentication** via **OpenSSL**.

This system includes:
- `server` : Remote command server (authentication + command execution)
- `client` : Remote client for sending commands
- `adduser`: Utility tool to add users into the user database (`users.txt`)

---

## 📌 Key Features

- **TCP socket-based client–server communication**
- **Secure Authentication using OpenSSL**
  - SHA-256 hashing with random salt
  - User credentials stored as: `username:salt:hash`
  - Session tokens issued after successful login
  - Token-based command authorization
- **Remote command execution**
  - Executes shell commands on the server and returns output
  - Supports `cd <path>` (directory switching handled specially)
  - Real-time output capture via pipes
- **Multi-client support**
  - Fork-based process model
  - Concurrent client handling
  - Automatic zombie process cleanup

---

## 🗂️ Project Structure

```
os-assignment/
│
├── include/
│   ├── Auth.h              # Authentication & hashing logic
│   ├── Client.h            # Client class interface
│   ├── CommandExecutor.h   # Command execution handler
│   ├── Colors.h            # Terminal color codes
│   ├── Server.h            # Server class interface
│   └── Socket.h            # RAII socket wrapper
│
├── src/
│   ├── socket/
│   │   └── Socket.cpp      # Socket implementation
│   │
│   ├── server/
│   │   ├── Auth.cpp        # SHA-256 + Salt authentication
│   │   ├── CommandExecutor.cpp  # Fork/exec/pipe command handling
│   │   ├── Server.cpp      # Server logic & client handling
│   │   ├── server_main.cpp # Server entry point
│   │   └── adduser_main.cpp     # User creation utility
│   │
│   └── client/
│       ├── Client.cpp      # Client implementation
│       └── client_main.cpp # Client entry point
│
├── data/
│   └── users.txt           # User database (salt:hash)
│
├── build/                  # Compiled binaries
├── Makefile               # Build configuration
├── README.md              # This file
├── ARCHITECTURE.md        # Detailed design document
└── README_DETAILED.md     # Extended documentation
```

---

## ⚙️ Technologies Used

- **C++17**
- **POSIX TCP sockets**
- **OpenSSL (`libssl`, `libcrypto`)**
- **pthread / concurrency support**
- Linux system APIs (`fork`, `exec`, `pipe`, `waitpid`, `chdir`, `getcwd`)

---

## ✅ Requirements

### OS
- Linux recommended (Ubuntu / Debian)

### Compiler
- `g++` with C++17 support

### Libraries
Install OpenSSL dev package:

```bash
sudo apt update
sudo apt install build-essential libssl-dev make
```

---

## 🔧 Build Instructions

Build all components:

```bash
make
```

Clean and rebuild:

```bash
make clean all
```

Build specific targets:

```bash
make server
make client
make adduser
```

---

## 👤 Add Users (Required)

Before connecting, create at least one user:

```bash
./adduser <username> <password>
```

Example:

```bash
./adduser admin password123
```

This will write credentials to:

```
data/users.txt
```

---

## ▶️ Running

### 1) Start the Server
```bash
./server          # Run in single-process mode
./server --fork   # Run with fork-based multi-client support (recommended)
```

### 2) Start the Client
```bash
./client
```

### 3) Login and Execute Commands
```
Enter username: admin
Enter password: ********
Authentication successful!

Remote Shell> ls
Remote Shell> pwd
Remote Shell> cd /tmp
Remote Shell> exit
```

---

## 🔐 Authentication Flow

1. **Connection:** Client connects to server
2. **Credentials:** Client sends `username:password`
3. **Verification:** Server:
   - Retrieves stored salt for username
   - Computes SHA-256 hash of `salt + password`
   - Compares with stored hash
4. **Response:** Server returns:
   - ✅ `AUTH_SUCCESS <session_token>` on success
   - ❌ `AUTH_FAILED <reason>` on failure
5. **Authorization:** All subsequent commands require valid token

---

## 💻 Remote Commands

After authentication, you can run commands like:

```bash
ls
pwd
whoami
date
uname -a
```

### `cd` support
```bash
cd /home
cd ..
cd ~/Downloads
```

---

## 🛡️ Security Notes

✅ Implemented:
- Passwords are not stored in plaintext
- Salted SHA-256 hashing via OpenSSL

⚠️ Possible future improvements:
- TLS encryption for all communication
- Argon2/bcrypt for stronger password hashing
- Command sandboxing / allowlist
- Rate limiting and auditing logs

---

## 🤝 Contributing

Contributions are welcome! Please see [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

Areas we'd especially appreciate help with:
- 🔒 Security enhancements (TLS/SSL, stronger hashing)
- 🧪 Testing (unit tests, integration tests)
- 📚 Documentation improvements
- 🚀 Performance optimizations

---

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

**Note:** Originally developed as a university coursework project, now open-sourced for educational purposes.

---

## 🌟 Acknowledgments

- Built using OpenSSL for cryptographic operations
- Inspired by traditional Unix remote shell utilities
- Educational project demonstrating OS concepts: processes, IPC, sockets, and authentication
