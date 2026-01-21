#include "Server.h"
#include <iostream>
#include <cstdlib>
#include <csignal>

// Global server 
Server* g_server = nullptr;

// Signal handler 
void signalHandler(int signum) {
    std::cout << "\nReceived signal " << signum << ". Shutting down..." << std::endl;
    if (g_server) {
        g_server->stop();
    }
    exit(signum);
}

int main(int argc, char* argv[]) {
    try {
        int port = 8080;
        bool use_fork = false;
        bool command_mode = false;
        
        for (int i = 1; i < argc; i++) {
            std::string arg = argv[i];
            
            if (arg == "-p" || arg == "--port") {
                if (i + 1 < argc) {
                    port = std::atoi(argv[++i]);
                }
            } else if (arg == "-f" || arg == "--fork") {
                use_fork = true;
            } else if (arg == "-c" || arg == "--command") {
                command_mode = true;
            } else if (arg == "-h" || arg == "--help") {
                std::cout << "Usage: " << argv[0] << " [options]\n"
                          << "Options:\n"
                          << "  -p, --port PORT     Port to listen on (default: 8080)\n"
                          << "  -f, --fork          Enable multi-client support with fork\n"
                          << "  -c, --command       Enable command execution mode\n"
                          << "  -h, --help          Show this help message\n";
                return 0;
            } else {
                port = std::atoi(argv[i]);
            }
        }
        
        // Create server
        Server server(port);
        g_server = &server;
        
        // Register signal handlers
        signal(SIGINT, signalHandler);
        signal(SIGTERM, signalHandler);
        
        // Configure server
        server.setUseFork(use_fork);
        
        // Start and run server
        server.start();
        server.run();
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
