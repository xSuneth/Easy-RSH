# Setup Wizard - User Guide

## First-Time Setup

When you run the server for the first time (or with `--reconfigure`), you'll be guided through an interactive setup wizard.

### Setup Flow

The wizard will ask you to configure:

1. **Admin Account**
   - Username (default: admin)
   - Password (hidden input)
   - Password confirmation

2. **Server Port**
   - Port number (default: 8080)
   - Automatically validates port availability

3. **Server Mode**
   - Command mode: Execute shell commands
   - Echo mode: Simple echo server

4. **Multi-Client Support**
   - Fork mode: Handle multiple clients simultaneously
   - Single client mode: One client at a time

5. **Save Configuration**
   - Save settings to `data/server.conf`
   - If not saved, you'll be asked to configure on each startup

### Example Session

```
  ███████╗ █████╗ ███████╗██╗   ██╗    ██████╗ ███████╗██╗  ██╗
  ██╔════╝██╔══██╗██╔════╝╚██╗ ██╔╝    ██╔══██╗██╔════╝██║  ██║
  █████╗  ███████║███████╗ ╚████╔╝     ██████╔╝███████╗███████║
  ██╔══╝  ██╔══██║╚════██║  ╚██╔╝      ██╔══██╗╚════██║██╔══██║
  ███████╗██║  ██║███████║   ██║       ██║  ██║███████║██║  ██║
  ╚══════╝╚═╝  ╚═╝╚══════╝   ╚═╝       ╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝
                    Easy Remote Shell Server v1.0

  Easy RSH Server setup wizard

  Let's configure your server.

┌ Admin Account
│
? Admin username (admin) › myuser
? Admin password › ********
? Confirm password › ********
⠙ Creating admin user
✔ Admin user created
└

┌ Server Port
│
? Port number (8080) › 9000
  ✔ Port 9000 is available
└

┌ Server Mode
│
? Select mode
  1) Command mode (execute shell commands)
  2) Echo mode (simple echo server)
  › 1
  ✔ Command mode selected
└

┌ Multi-Client Support
│
? Support multiple clients?
  1) Yes - Fork mode (multiple clients)
  2) No - Single client mode
  › 1
  ✔ Fork mode enabled
└

? Save configuration? (Y/n) › Y
⠸ Saving configuration
✔ Configuration saved to data/server.conf

✔ Setup complete!
```

## Running the Server

### First Time
```bash
./server
```
This will automatically launch the setup wizard.

### Subsequent Runs
```bash
./server  # Uses saved configuration
```

### Reconfigure
```bash
./server --reconfigure  # Re-run setup wizard
```

### Override Configuration
```bash
./server -p 9000 -c -f  # Override port, enable command & fork modes
```

## Command-Line Options

| Option | Description |
|--------|-------------|
| `-p, --port PORT` | Override configured port |
| `-c, --command` | Enable command execution mode |
| `-f, --fork` | Enable multi-client fork mode |
| `--reconfigure` | Re-run setup wizard |
| `-h, --help` | Show help message |

## Configuration File

Location: `data/server.conf`

Example:
```ini
# Easy RSH Server Configuration
# Generated automatically - edit with caution

[server]
port=8080
command_mode=true
use_fork=true
first_run=false
```

## Features

- ✨ **Clean Vite-like CLI** - Minimal, modern interface
- 🎨 **Custom Theme Color** - rgb(175, 135, 255) purple theme
- ⚡ **Animated Spinners** - Visual feedback during operations
- 🔒 **Secure Admin Setup** - Create admin account on first run
- 💾 **Persistent Configuration** - Save preferences for future use
- 🔄 **Easy Reconfiguration** - Change settings anytime
- ✅ **Port Validation** - Checks port availability automatically
- 🧹 **Clear Terminal** - Fresh start on each launch

## Tips

- Press Enter to accept default values (shown in parentheses)
- Passwords are hidden for security
- The terminal clears automatically on startup for a clean experience
- If you don't save configuration, you'll be prompted on each startup
- Use `--reconfigure` to change settings after initial setup
