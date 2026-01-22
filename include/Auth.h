#ifndef AUTH_H
#define AUTH_H

#include <string>
#include <map>
#include <chrono>

/**
 * Authentication module for user authentication and session management
 * Uses SHA-256 hashing with salt for password storage
 */
class Auth {
public:
    struct Session {
        std::string username;
        std::string token;
        std::chrono::system_clock::time_point created_at;
        std::chrono::system_clock::time_point last_activity;
        bool is_valid;
    };

private:
    std::string users_file_;
    std::map<std::string, std::string> users_;  // username -> password_hash
    std::map<std::string, Session> sessions_;   // token -> session
    int session_timeout_minutes_;

    // Load users from file
    void loadUsers();

    // Hash password with salt using SHA-256
    static std::string hashPassword(const std::string& password, const std::string& salt);

    // Generate random salt
    static std::string generateSalt();

    // Generate random session token
    static std::string generateToken();

    // Check if session is expired
    bool isSessionExpired(const Session& session) const;

public:
    // Constructor
    explicit Auth(const std::string& users_file = "data/users.txt", 
                  int timeout_minutes = 30);

    // Authenticate user with username and password
    // Returns session token if successful, empty string if failed
    std::string authenticate(const std::string& username, const std::string& password);

    // Validate a session token
    // Returns true if token is valid and not expired
    bool validateToken(const std::string& token);

    // Get username from session token
    std::string getUsernameFromToken(const std::string& token);

    // Revoke/logout a session
    void revokeToken(const std::string& token);

    // Update session activity timestamp (for keeping session alive)
    void updateActivity(const std::string& token);

    // Clean up expired sessions
    void cleanupExpiredSessions();

    // Add a new user (for administration)
    bool addUser(const std::string& username, const std::string& password);

    // Save users to file
    void saveUsers();

    // Get session count
    size_t getActiveSessionCount() const;
};

#endif // AUTH_H
