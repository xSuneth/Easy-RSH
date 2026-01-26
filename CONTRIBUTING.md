# Contributing to Easy Remote Shell (Easy RSH)

Thank you for considering contributing to this project! 🎉

## How to Contribute

### Reporting Bugs

If you find a bug, please open an issue with:
- A clear description of the problem
- Steps to reproduce
- Expected vs actual behavior
- Your environment (OS, compiler version, OpenSSL version)

### Suggesting Enhancements

We welcome feature suggestions! Please open an issue describing:
- The problem you're trying to solve
- Your proposed solution
- Any alternatives you've considered

### Pull Requests

1. **Fork the repository**
2. **Create a feature branch**
   ```bash
   git checkout -b feature/your-feature-name
   ```

3. **Make your changes**
   - Follow the existing code style
   - Add comments for complex logic
   - Update documentation if needed

4. **Test your changes**
   ```bash
   make clean all
   ./server --fork &
   ./client
   ```

5. **Commit with meaningful messages**
   ```bash
   git commit -m "feat: Add your feature description"
   ```
   
   Use conventional commits:
   - `feat:` - New feature
   - `fix:` - Bug fix
   - `docs:` - Documentation changes
   - `refactor:` - Code refactoring
   - `test:` - Adding tests
   - `chore:` - Maintenance tasks

6. **Push and create a Pull Request**
   ```bash
   git push origin feature/your-feature-name
   ```

## Code Style Guidelines

- Use C++17 standard features
- Follow RAII principles for resource management
- Prefer `nullptr` over `NULL`
- Use meaningful variable names
- Add error handling for system calls
- Keep functions focused and concise

## Development Setup

### Prerequisites
```bash
sudo apt update
sudo apt install build-essential libssl-dev make
```

### Building
```bash
make clean all
```

### Testing
```bash
# Terminal 1
./server --fork

# Terminal 2
./adduser testuser testpass
./client
```

## Areas for Contribution

We especially welcome contributions in:

- 🔒 **Security Enhancements**
  - TLS/SSL encryption
  - Stronger password hashing (Argon2/bcrypt)
  - Command sandboxing
  - Rate limiting

- 🚀 **Performance**
  - Thread pool implementation
  - Event-driven I/O (epoll/kqueue)
  - Connection pooling

- 📝 **Documentation**
  - Additional examples
  - Tutorials
  - Architecture diagrams
  - API documentation

- ✅ **Testing**
  - Unit tests
  - Integration tests
  - Load testing

- 🛠️ **Features**
  - File transfer support
  - Command history
  - Shell tab completion
  - Multi-platform support (macOS, Windows WSL)

## Questions?

Feel free to open an issue for any questions about contributing!

## Code of Conduct

Be respectful, inclusive, and constructive in all interactions.

## License

By contributing, you agree that your contributions will be licensed under the MIT License.
