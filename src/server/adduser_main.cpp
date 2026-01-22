#include "Auth.h"
#include <iostream>
#include <string>

/**
 * User management utility
 * Usage: ./adduser <username> <password>
 */

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " <username> <password>" << std::endl;
        std::cout << "Example: " << argv[0] << " admin password123" << std::endl;
        return 1;
    }

    std::string username = argv[1];
    std::string password = argv[2];

    try {
        // Create Auth instance
        Auth auth("data/users.txt");

        // Add user
        if (auth.addUser(username, password)) {
            auth.saveUsers();
            std::cout << "\n✓ User '" << username << "' added successfully!" << std::endl;
            std::cout << "Users file: data/users.txt" << std::endl;
            return 0;
        } else {
            std::cerr << "\n✗ Failed to add user '" << username << "'" << std::endl;
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
