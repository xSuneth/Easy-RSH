# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.0.0] - 2026-01-26

### Added
- Remote command execution system with client-server architecture
- SHA-256 + Salt password authentication via OpenSSL
- Session token-based authorization
- Multi-client support using fork-based process model
- `adduser` utility for user management
- Special handling for `cd` command with directory state preservation
- Automatic zombie process cleanup with SIGCHLD handler
- RAII Socket wrapper class for resource management
- Command execution using fork/exec/pipe pattern
- Interactive shell interface for clients

### Security
- Implemented salted SHA-256 password hashing
- User credentials stored securely (no plaintext passwords)
- Session token generation using cryptographically secure random
- Token validation for all command execution requests

### Documentation
- Complete README with setup and usage instructions
- Detailed ARCHITECTURE.md with system design documentation
- Code comments and inline documentation
- Process hierarchy and communication flow diagrams

## [Unreleased]

### Planned
- TLS/SSL encryption for network communication
- Command whitelisting and sandboxing
- Rate limiting and brute-force protection
- Audit logging system
- Stronger password hashing (Argon2/bcrypt)
- Persistent session management

---

## Version Format

This project uses [Semantic Versioning](https://semver.org/):
- **MAJOR** version: Incompatible API changes
- **MINOR** version: New functionality (backward compatible)
- **PATCH** version: Bug fixes (backward compatible)
