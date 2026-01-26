#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <map>

class Config {
private:
    std::string config_file_;
    std::map<std::string, std::string> settings_;
    bool loaded_;

    // Parse a line from config file
    void parseLine(const std::string& line);
    
    // Trim whitespace from string
    std::string trim(const std::string& str) const;

public:
    explicit Config(const std::string& config_file = "data/server.conf");
    
    // Check if config file exists
    bool exists() const;
    
    // Load configuration from file
    bool load();
    
    // Save configuration to file
    bool save();
    
    // Get a setting value
    std::string get(const std::string& key, const std::string& default_value = "") const;
    
    // Set a setting value
    void set(const std::string& key, const std::string& value);
    
    // Get boolean value
    bool getBool(const std::string& key, bool default_value = false) const;
    
    // Set boolean value
    void setBool(const std::string& key, bool value);
    
    // Get integer value
    int getInt(const std::string& key, int default_value = 0) const;
    
    // Set integer value
    void setInt(const std::string& key, int value);
    
    // Check if first run (no config or first_run flag set)
    bool isFirstRun();
    
    // Mark setup as complete
    void markSetupComplete();
};

#endif // CONFIG_H
