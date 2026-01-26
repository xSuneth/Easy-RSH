#include "CLIUtils.h"
#include "Colors.h"
#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <chrono>

namespace CLI {

void clear() {
    std::cout << "\033[2J\033[H" << std::flush;
}

void home() {
    std::cout << "\033[H" << std::flush;
}

void hideCursor() {
    std::cout << "\033[?25l" << std::flush;
}

void showCursor() {
    std::cout << "\033[?25h" << std::flush;
}

char getch() {
    char buf = 0;
    struct termios old = {0};
    if (tcgetattr(0, &old) < 0) {
        perror("tcsetattr()");
    }
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0) {
        perror("tcsetattr ICANON");
    }
    if (read(0, &buf, 1) < 0) {
        perror("read()");
    }
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0) {
        perror("tcsetattr ~ICANON");
    }
    return buf;
}

// Spinner implementation
Spinner::Spinner(const std::string& message)
    : message_(message), running_(false), current_frame_(0) {
    // Simple dots spinner
    frames_ = {"⠋", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠧", "⠇", "⠏"};
}

Spinner::~Spinner() {
    if (running_) {
        stop();
    }
}

void Spinner::spin() {
    hideCursor();
    while (running_) {
        std::cout << "\r" << Color::THEME << frames_[current_frame_] << Color::RESET 
                  << " " << message_ << std::flush;
        current_frame_ = (current_frame_ + 1) % frames_.size();
        std::this_thread::sleep_for(std::chrono::milliseconds(80));
    }
    showCursor();
}

void Spinner::start() {
    if (!running_) {
        running_ = true;
        thread_ = std::thread(&Spinner::spin, this);
    }
}

void Spinner::stop(const std::string& final_message) {
    if (running_) {
        running_ = false;
        if (thread_.joinable()) {
            thread_.join();
        }
        std::cout << "\r\033[K";  // Clear line
        if (!final_message.empty()) {
            std::cout << final_message << std::endl;
        }
    }
}

std::string prompt(const std::string& message, const std::string& default_value) {
    std::cout << Color::THEME << "? " << Color::RESET << message;
    
    if (!default_value.empty()) {
        std::cout << Color::DIM << " (" << default_value << ")" << Color::RESET;
    }
    
    std::cout << Color::THEME << " › " << Color::RESET;
    
    std::string input;
    std::getline(std::cin, input);
    
    // Trim input
    size_t start = input.find_first_not_of(" \t");
    size_t end = input.find_last_not_of(" \t");
    
    if (start == std::string::npos) {
        return default_value;
    }
    
    input = input.substr(start, end - start + 1);
    
    return input.empty() ? default_value : input;
}

std::string promptPassword(const std::string& message) {
    std::cout << Color::THEME << "? " << Color::RESET << message 
              << Color::THEME << " › " << Color::RESET;
    
    // Disable echo
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    
    std::string password;
    std::getline(std::cin, password);
    
    // Re-enable echo
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    
    std::cout << std::endl;
    
    return password;
}

bool promptYesNo(const std::string& message, bool default_yes) {
    std::string prompt_text = message;
    if (default_yes) {
        prompt_text += " (Y/n)";
    } else {
        prompt_text += " (y/N)";
    }
    
    std::string response = prompt(prompt_text, default_yes ? "y" : "n");
    
    if (response.empty()) {
        return default_yes;
    }
    
    char first = tolower(response[0]);
    return (first == 'y');
}

int promptSelect(const std::string& message, const std::vector<std::string>& options) {
    std::cout << Color::THEME << "? " << Color::RESET << message << std::endl;
    
    int selected = 0;
    bool done = false;
    
    // Setup terminal for raw mode
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    
    hideCursor();
    
    while (!done) {
        // Display options
        for (size_t i = 0; i < options.size(); ++i) {
            std::cout << "\r";
            if (i == static_cast<size_t>(selected)) {
                // Highlight selected option
                std::cout << "  " << Color::THEME << "◉" << Color::RESET 
                          << " " << options[i];
            } else {
                // Unselected option
                std::cout << "  " << Color::DIM << "○" << Color::RESET 
                          << " " << Color::DIM << options[i] << Color::RESET;
            }
            std::cout << "\033[K" << std::endl;  // Clear to end of line
        }
        
        // Read key
        char c = getchar();
        
        if (c == '\033') {  // Escape sequence
            getchar();  // Skip '['
            char arrow = getchar();
            
            if (arrow == 'A') {  // Up arrow
                selected = (selected - 1 + options.size()) % options.size();
            } else if (arrow == 'B') {  // Down arrow
                selected = (selected + 1) % options.size();
            }
        } else if (c == '\n' || c == '\r') {  // Enter
            done = true;
        } else if (c >= '1' && c <= '9') {  // Number key
            int num = c - '0';
            if (num > 0 && num <= static_cast<int>(options.size())) {
                selected = num - 1;
                done = true;
            }
        }
        
        // Move cursor back up
        if (!done) {
            for (size_t i = 0; i < options.size(); ++i) {
                std::cout << "\033[A";  // Move up
            }
        }
    }
    
    // Move cursor back to first option line
    for (size_t i = 0; i < options.size(); ++i) {
        std::cout << "\033[A";
    }
    
    // Clear all option lines
    for (size_t i = 0; i < options.size(); ++i) {
        std::cout << "\r\033[K";
        if (i < options.size() - 1) {
            std::cout << "\n";
        }
    }
    
    // Move back to first line
    for (size_t i = 1; i < options.size(); ++i) {
        std::cout << "\033[A";
    }
    
    // Show final selected option
    std::cout << "\r  " << Color::THEME << "◉" << Color::RESET 
              << " " << options[selected] << "\033[K" << std::endl;
    
    // Restore terminal
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    showCursor();
    
    return selected;
}

void success(const std::string& message) {
    std::cout << Color::GREEN << "✔ " << Color::RESET << message << std::endl;
}

void error(const std::string& message) {
    std::cout << Color::ROSE << "✖ " << Color::RESET << message << std::endl;
}

void info(const std::string& message) {
    std::cout << Color::THEME << "ℹ " << Color::RESET << message << std::endl;
}

}  // namespace CLI
