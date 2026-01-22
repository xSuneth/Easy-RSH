# Remote Command Execution System (Client–Server) with OpenSSL Authentication


 ███████╗ █████╗ ███████╗██╗   ██╗    ██████╗ ███████╗██╗  ██╗
 ██╔════╝██╔══██╗██╔════╝╚██╗ ██╔╝    ██╔══██╗██╔════╝██║  ██║
 █████╗  ███████║███████╗ ╚████╔╝     ██████╔╝███████╗███████║
 ██╔══╝  ██╔══██║╚════██║  ╚██╔╝      ██╔══██╗╚════██║██╔══██║
 ███████╗██║  ██║███████║   ██║       ██║  ██║███████║██║  ██║
 ╚══════╝╚═╝  ╚═╝╚══════╝   ╚═╝       ╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝
 Easy Remote Shell Server v1.0"
 

A C++ TCP **client–server command execution system** that allows a remote client to connect to a server and execute shell commands securely using **SHA-256 + Salt password authentication** via **OpenSSL**.

This system includes:
- `server` : Remote command server (authentication + command execution)
- `client` : Remote client for sending commands
- `adduser`: Utility tool to add users into the user database (`users.txt`)

---

## 📌 Key Features

- **TCP socket-based client–server communication**
- **Authentication using OpenSSL**
  - SHA-256 hashing
  - Random salt generation
  - User credentials stored as: `username:salt:hash`
- **Remote command execution**
  - Executes shell commands on the server and returns output
  - Supports `cd <path>` (directory switching handled specially)
- **Session token issued after successful login**

---

## 🗂️ Project Structure

```
projects-team-1/
│
├── include/
│   ├── Auth.h
│   ├── Client.h
│   ├── CommandExecutor.h
│   ├── Colors.h
│   ├── Server.h
│   └── Socket.h
│
├── src/
│   ├── socket/
│   │   └── Socket.cpp
│   │
│   ├── server/
│   │   ├── Auth.cpp
│   │   ├── CommandExecutor.cpp
│   │   ├── Server.cpp
│   │   ├── server_main.cpp
│   │   └── adduser_main.cpp
│   │
│   └── client/
│       ├── Client.cpp
│       └── client_main.cpp
│
├── data/
│   └── users.txt
│
├── build/
├── Makefile
└── Documentation PDFs
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
./server
```

### 2) Start the Client
```bash
./client
```

---

## 🔐 Authentication Flow (High Level)

1. Server requests authentication
2. Client sends `username:password`
3. Server verifies using `salt + SHA-256 hash`
4. Server returns:
   - ✅ `AUTH_SUCCESS <token>`
   - ❌ `AUTH_FAILED ...`

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

## 📄 License
Educational project (University coursework).
