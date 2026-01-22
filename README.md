# OpenSSL Authentication Implementation

## Overview
User authentication using SHA-256 hashing with salt has been successfully implemented using OpenSSL.

## Features Implemented

### 1. **Authentication Module (`Auth.h`, `Auth.cpp`)**
- SHA-256 password hashing with random salt
- Session token generation and management
- Session timeout (default: 30 minutes)
- User management (add, authenticate, validate)

### 2. **Server-Side Authentication**
- Authentication handshake before accepting commands
- Session token validation
- Configurable authentication requirement

### 3. **Client-Side Authentication**
- Automatic authentication during connection
- Username/password prompt
- Session token storage

### 4. **User Management Utility (`adduser`)**
- Command-line tool to add users
- Secure password hashing
- Users stored in `data/users.txt`

## Usage

### Building the Project

```bash
make clean all
```

This builds:
- `server` - The command execution server with authentication
- `client` - The client with authentication support
- `adduser` - User management utility

### Managing Users

#### Add a new user:
```bash
./adduser <username> <password>

# Example:
./adduser admin admin123
./adduser alice secretpass
```

#### View users (hashed):
```bash
cat data/users.txt
```

### Running the Server

```bash
# Run with authentication (default)
./server -p 8080

# The server now requires authentication by default
```

### Running the Client

```bash
# Connect to server
./client -h 192.168.8.109 -p 8080

# You will be prompted:
# Username: admin
# Password: admin123
```

## Security Features

### Password Security
- **Salted hashing**: Each password has a unique random salt
- **SHA-256**: Cryptographically secure hash function
- **No plain text**: Passwords never stored in plain text

### Session Security
- **Random tokens**: 256-bit cryptographically random session tokens
- **Session timeout**: Automatic expiration after inactivity
- **Token validation**: Every command validates the session token

### Authentication Flow

```
Client                          Server
  |                               |
  |------- Connect --------------->|
  |<------ AUTH_REQUIRED ----------|
  |                               |
  |-- AUTH username:password ----->|
  |                               | (verify credentials)
  |<-- AUTH_SUCCESS token ---------|
  |                               |
  |-- CMD command ---------------->|
  |                               | (validate token)
  |<----- RESULT output ----------|
```

## File Structure

```
include/
  └── Auth.h              # Authentication module header

src/server/
  ├── Auth.cpp           # Authentication implementation
  ├── adduser_main.cpp   # User management utility
  └── Server.cpp         # Updated with auth integration

src/client/
  └── Client.cpp         # Updated with auth client

data/
  └── users.txt          # User credentials (auto-created)
```

## Configuration

### Server Configuration
You can disable authentication (not recommended):
```cpp
Server server(8080);
server.setRequireAuth(false);  // Disable authentication
```

### Session Timeout
Default is 30 minutes. To change:
```cpp
Auth auth("data/users.txt", 60);  // 60 minutes timeout
```

## Testing

1. **Create test users:**
   ```bash
   ./adduser admin admin123
   ./adduser testuser password123
   ```

2. **Start server:**
   ```bash
   ./server -p 8080
   ```

3. **Connect client:**
   ```bash
   ./client -h localhost -p 8080
   # Enter credentials when prompted
   ```

4. **Test authentication failure:**
   - Try wrong password
   - Should see "Authentication failed" message

## Security Best Practices

### Implemented ✅
- Salted password hashing
- Secure random token generation
- Session timeout
- No password transmission in plain text (within auth message)

### Recommended Additions 🔐
1. **TLS/SSL encryption** - Encrypt all network traffic
2. **Password complexity requirements** - Enforce strong passwords
3. **Rate limiting** - Prevent brute force attacks
4. **Audit logging** - Log all authentication attempts
5. **Password hiding** - Use termios to hide password input in client

## Dependencies

- **OpenSSL** (`libssl-dev`):
  ```bash
  sudo apt-get install libssl-dev
  ```

## Troubleshooting

### Build Error: "openssl/sha.h: No such file or directory"
```bash
sudo apt-get install libssl-dev
```

### Authentication Always Fails
- Check users file exists: `ls -la data/users.txt`
- Verify user was added: `cat data/users.txt`
- Ensure correct username/password

### "User not found"
- Add user: `./adduser <username> <password>`

## API Reference

### Auth Class

#### Constructor
```cpp
Auth(const std::string& users_file = "data/users.txt", 
     int timeout_minutes = 30);
```

#### Methods
```cpp
// Authenticate user, returns session token
std::string authenticate(const std::string& username, 
                        const std::string& password);

// Validate session token
bool validateToken(const std::string& token);

// Add new user
bool addUser(const std::string& username, 
            const std::string& password);

// Save users to file
void saveUsers();

// Revoke session
void revokeToken(const std::string& token);

// Update session activity
void updateActivity(const std::string& token);

// Clean expired sessions
void cleanupExpiredSessions();
```

## Next Steps

To further enhance security:

1. **Add TLS/SSL**: Encrypt all client-server communication
2. **Command whitelisting**: Restrict allowed commands per user
3. **Role-based access control**: Different permission levels
4. **Audit logging**: Track all user actions
5. **Password strength validation**: Enforce complexity rules
6. **Multi-factor authentication**: Add second factor
7. **Password reset mechanism**: Secure password recovery

---

**Status**: ✅ OpenSSL authentication fully implemented and tested
**Date**: January 22, 2026
