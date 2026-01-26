#ifndef SETUPWIZARD_H
#define SETUPWIZARD_H

#include "Config.h"
#include "Auth.h"
#include <string>
#include <memory>

class SetupWizard {
private:
    Config& config_;
    std::shared_ptr<Auth> auth_;
    
    // Print welcome message
    void printWelcome();
    
    // Print completion message
    void printComplete();
    
    // Setup admin user
    bool setupAdmin();
    
    // Setup server port
    bool setupPort();
    
    // Setup server mode
    bool setupMode();
    
    // Setup multi-client support
    bool setupFork();
    
    // Ask to save configuration
    bool askSaveConfig();

public:
    SetupWizard(Config& config, std::shared_ptr<Auth> auth);
    
    // Run the setup wizard
    bool run();
};

#endif // SETUPWIZARD_H
