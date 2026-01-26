#ifndef CLIUTILS_H
#define CLIUTILS_H

#include <string>
#include <vector>
#include <functional>
#include <thread>
#include <atomic>

namespace CLI {
    // Clear the terminal screen
    void clear();
    
    // Move cursor to home position
    void home();
    
    // Hide cursor
    void hideCursor();
    
    // Show cursor
    void showCursor();
    
    // Get a single character without echo
    char getch();
    
    // Spinner class for async operations
    class Spinner {
    private:
        std::string message_;
        std::atomic<bool> running_;
        std::thread thread_;
        std::vector<std::string> frames_;
        int current_frame_;
        
        void spin();
        
    public:
        explicit Spinner(const std::string& message = "");
        ~Spinner();
        
        void start();
        void stop(const std::string& final_message = "");
    };
    
    // Prompt user for text input
    std::string prompt(const std::string& message, const std::string& default_value = "");
    
    // Prompt user for password (hidden input)
    std::string promptPassword(const std::string& message);
    
    // Prompt user with yes/no question
    bool promptYesNo(const std::string& message, bool default_yes = true);
    
    // Prompt user to select from a list of options
    int promptSelect(const std::string& message, const std::vector<std::string>& options);
    
    // Display success message
    void success(const std::string& message);
    
    // Display error message
    void error(const std::string& message);
    
    // Display info message
    void info(const std::string& message);
}

#endif // CLIUTILS_H
