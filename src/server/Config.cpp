#include "Config.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <sys/stat.h>

Config::Config(const std::string& config_file)
    : config_file_(config_file), loaded_(false) {}

bool Config::exists() const {
    struct stat buffer;
    return (stat(config_file_.c_str(), &buffer) == 0);
}

std::string Config::trim(const std::string& str) const {
    size_t start = str.find_first_not_of(" \t\n\r");
    size_t end = str.find_last_not_of(" \t\n\r");
    
    if (start == std::string::npos) {
        return "";
    }
    
    return str.substr(start, end - start + 1);
}

void Config::parseLine(const std::string& line) {
    // Skip empty lines and comments
    std::string trimmed = trim(line);
    if (trimmed.empty() || trimmed[0] == '#' || trimmed[0] == '[') {
        return;
    }
    
    // Parse key=value
    size_t equals_pos = trimmed.find('=');
    if (equals_pos != std::string::npos) {
        std::string key = trim(trimmed.substr(0, equals_pos));
        std::string value = trim(trimmed.substr(equals_pos + 1));
        settings_[key] = value;
    }
}

bool Config::load() {
    if (!exists()) {
        return false;
    }
    
    std::ifstream file(config_file_);
    if (!file.is_open()) {
        return false;
    }
    
    settings_.clear();
    std::string line;
    while (std::getline(file, line)) {
        parseLine(line);
    }
    
    file.close();
    loaded_ = true;
    return true;
}

bool Config::save() {
    // Create data directory if it doesn't exist
    mkdir("data", 0755);
    
    std::ofstream file(config_file_);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# Easy RSH Server Configuration\n";
    file << "# Generated automatically - edit with caution\n\n";
    
    file << "[server]\n";
    for (const auto& [key, value] : settings_) {
        file << key << "=" << value << "\n";
    }
    
    file.close();
    return true;
}

std::string Config::get(const std::string& key, const std::string& default_value) const {
    auto it = settings_.find(key);
    if (it != settings_.end()) {
        return it->second;
    }
    return default_value;
}

void Config::set(const std::string& key, const std::string& value) {
    settings_[key] = value;
}

bool Config::getBool(const std::string& key, bool default_value) const {
    std::string value = get(key, "");
    if (value.empty()) {
        return default_value;
    }
    
    std::string lower = value;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    
    return (lower == "true" || lower == "yes" || lower == "1" || lower == "on");
}

void Config::setBool(const std::string& key, bool value) {
    set(key, value ? "true" : "false");
}

int Config::getInt(const std::string& key, int default_value) const {
    std::string value = get(key, "");
    if (value.empty()) {
        return default_value;
    }
    
    try {
        return std::stoi(value);
    } catch (...) {
        return default_value;
    }
}

void Config::setInt(const std::string& key, int value) {
    set(key, std::to_string(value));
}

bool Config::isFirstRun() {
    if (!exists()) {
        return true;
    }
    
    // Load config if not already loaded
    if (!loaded_) {
        load();
    }
    
    // Check if first_run flag is set
    return getBool("first_run", true);
}

void Config::markSetupComplete() {
    setBool("first_run", false);
}
