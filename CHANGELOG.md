# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.1.0] - 2026-01-27

### Added
- **Interactive Setup Wizard** - Vite-like CLI for first-time configuration
  - Admin user creation with password confirmation
  - Server port selection with availability validation
  - Server mode selection (Command/Echo)
  - Multi-client fork mode configuration
  - Configuration persistence to `data/server.conf`
- **Enhanced CLI/UX**
  - Custom theme color RGB(175, 135, 255) for minimal purple theme
  - Interactive arrow-key navigation for radio button selections
  - Animated spinner for "Waiting for connection" (`/ - \ |`)
  - Loading spinners for async operations (user creation, config save)
  - Terminal clearing on startup for clean experience
- **Formatted Server Messages**
  - Color-coded connection messages with arrows (→)
  - Authentication status with visual indicators (✔/✖)
  - Clean, minimal message formatting
  - Real-time authentication spinner (⏳)
- **Configuration Management**
  - Config class for INI-style configuration files
  - Automatic first-run detection
  - `--reconfigure` flag to re-run setup wizard
  - Command-line flags still override saved config
- **CLI Utilities**
  - Prompt functions with default values
  - Hidden password input
  - Yes/No prompts
  - Interactive selection with arrow keys
  - Success/Error message helpers
  - Cursor hide/show utilities

### Changed
- Updated banner version from v1.0 to v1.1.0
- Server startup now checks for config and runs wizard if needed
- Authentication messages are now silent in Auth class, formatted by Server
- Improved .gitignore to exclude config and build artifacts

### Fixed
- Command mode not being applied when selected in setup wizard
- Config not loading after wizard completion
- Double display of selected option in radio buttons
- First-run detection not working properly
- Empty lines in setup wizard sections

### Improved
- Minimal, clean Vite-like interface
- Professional color scheme with theme purple
- Better user experience with visual feedback
- Clear separation between setup and runtime phases

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
