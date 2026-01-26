#include "SetupWizard.h"
#include "CLIUtils.h"
#include "Colors.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

SetupWizard::SetupWizard(Config& config, std::shared_ptr<Auth> auth)
    : config_(config), auth_(auth) {}

void SetupWizard::printWelcome() {
    std::cout << "\n";
    std::cout << Color::THEME << "  Easy RSH Server" << Color::RESET << Color::DIM << " setup wizard" << Color::RESET << "\n\n";
    std::cout << Color::DIM << "  Let's configure your server.\n" << Color::RESET;
    std::cout << "\n";
}

void SetupWizard::printComplete() {
    std::cout << "\n";
    CLI::success("Setup complete!");
    std::cout << "\n";
}

bool SetupWizard::setupAdmin() {
    std::cout << Color::DIM << "┌ Admin Account" << Color::RESET << "\n";
    
    std::string username = CLI::prompt("Admin username", "admin");
    
    if (username.empty()) {
        CLI::error("Username cannot be empty");
        return false;
    }
    
    std::string password = CLI::promptPassword("Admin password");
    
    if (password.empty()) {
        CLI::error("Password cannot be empty");
        return false;
    }
    
    std::string confirm = CLI::promptPassword("Confirm password");
    
    if (password != confirm) {
        CLI::error("Passwords do not match");
        return false;
    }
    
    // Create admin user
    CLI::Spinner spinner("Creating admin user");
    spinner.start();
    
    std::this_thread::sleep_for(std::chrono::milliseconds(500));  // Visual feedback
    
    if (auth_->addUser(username, password)) {
        auth_->saveUsers();  // Save to file
        spinner.stop(Color::GREEN + std::string("✔") + Color::RESET + " Admin user created");
    } else {
        spinner.stop(Color::ROSE + std::string("✖") + Color::RESET + " Failed to create admin user");
        return false;
    }
    
    std::cout << Color::DIM << "└" << Color::RESET << "\n\n";
    return true;
}

bool SetupWizard::setupPort() {
    std::cout << Color::DIM << "┌ Server Port" << Color::RESET << "\n";
    
    std::string port_str = CLI::prompt("Port number", "8080");
    int port = 8080;
    
    try {
        port = std::stoi(port_str);
    } catch (...) {
        port = 8080;
    }
    
    // Validate port
    if (port < 1024 || port > 65535) {
        CLI::error("Port must be between 1024 and 65535");
        return false;
    }
    
    // Quick check if port is available
    int test_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (test_socket >= 0) {
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(port);
        
        int reuse = 1;
        setsockopt(test_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
        
        if (bind(test_socket, (struct sockaddr*)&addr, sizeof(addr)) == 0) {
            std::cout << Color::GREEN << "  ✔" << Color::RESET << " Port " << port << " is available\n";
            close(test_socket);
        } else {
            close(test_socket);
            CLI::error("Port " + std::to_string(port) + " is already in use");
            return false;
        }
    }
    
    config_.setInt("port", port);
    std::cout << Color::DIM << "└" << Color::RESET << "\n\n";
    return true;
}

bool SetupWizard::setupMode() {
    std::cout << Color::DIM << "┌ Server Mode" << Color::RESET << "\n";
    
    std::vector<std::string> modes = {
        "Command mode (execute shell commands)",
        "Echo mode (simple echo server)"
    };
    
    int selected = CLI::promptSelect("Select mode", modes);
    
    bool command_mode = (selected == 0);
    config_.setBool("command_mode", command_mode);
    
    std::cout << Color::GREEN << "  ✔" << Color::RESET << " " 
              << (command_mode ? "Command" : "Echo") << " mode selected\n";
    
    std::cout << Color::DIM << "└" << Color::RESET << "\n\n";
    return true;
}

bool SetupWizard::setupFork() {
    std::cout << Color::DIM << "┌ Multi-Client Support" << Color::RESET << "\n";
    
    std::vector<std::string> options = {
        "Yes - Fork mode (multiple clients)",
        "No - Single client mode"
    };
    
    int selected = CLI::promptSelect("Support multiple clients?", options);
    
    bool use_fork = (selected == 0);
    config_.setBool("use_fork", use_fork);
    
    std::cout << Color::GREEN << "  ✔" << Color::RESET << " " 
              << (use_fork ? "Fork mode enabled" : "Single client mode") << "\n";
    
    std::cout << Color::DIM << "└" << Color::RESET << "\n\n";
    return true;
}

bool SetupWizard::askSaveConfig() {
    bool save = CLI::promptYesNo("Save configuration?", true);
    
    if (save) {
        config_.markSetupComplete();
        
        CLI::Spinner spinner("Saving configuration");
        spinner.start();
        
        std::this_thread::sleep_for(std::chrono::milliseconds(400));
        
        bool saved = config_.save();
        
        if (saved) {
            spinner.stop(Color::GREEN + std::string("✔") + Color::RESET + " Configuration saved to data/server.conf");
        } else {
            spinner.stop(Color::ROSE + std::string("✖") + Color::RESET + " Failed to save configuration");
            return false;
        }
    } else {
        CLI::info("Configuration not saved. You'll need to configure on next startup.");
    }
    
    return true;
}

bool SetupWizard::run() {
    CLI::clear();
    printWelcome();
    
    // Step 1: Admin account
    if (!setupAdmin()) {
        return false;
    }
    
    // Step 2: Port
    if (!setupPort()) {
        return false;
    }
    
    // Step 3: Server mode
    if (!setupMode()) {
        return false;
    }
    
    // Step 4: Fork mode
    if (!setupFork()) {
        return false;
    }
    
    // Step 5: Save config
    if (!askSaveConfig()) {
        return false;
    }
    
    printComplete();
    
    return true;
}
