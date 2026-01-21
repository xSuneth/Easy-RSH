#ifndef COMMANDEXECUTOR_H
#define COMMANDEXECUTOR_H

#include <string>
#include <vector>

class CommandExecutor {
public:
    struct Result {
        std::string output;
        int exit_code;
        bool success;
    };
    
    // Execute a command and capture the output
    static Result execute(const std::string& command);
    
private:
    // Parse command string into array
    static std::vector<std::string> parseCommand(const std::string& command);
    
    // Read all data from a file descriptor
    static std::string readFromPipe(int fd);
};

#endif 
