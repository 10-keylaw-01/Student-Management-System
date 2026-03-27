#pragma once
#include <string>
#include <map>
#include <chrono>
#include <algorithm>
#include <iostream>
#include "Crypto.h"

/// Authentication manager with lockout protection
class Auth {
    // username -> {attempt_count, lockout_time_point}
    std::map<std::string, std::pair<int, std::chrono::steady_clock::time_point>> attempts;
    static const int MAX_ATTEMPTS  = 3;
    static const int LOCKOUT_SECS  = 900; // 15 minutes

public:
    /// Check if username is currently locked out after failed login attempts
    bool isLockedOut(const std::string& username) {
        auto it = attempts.find(username);
        if (it == attempts.end()) return false;
        if (it->second.first < MAX_ATTEMPTS) return false;
        auto elapsed = std::chrono::steady_clock::now() - it->second.second;
        if (std::chrono::duration_cast<std::chrono::seconds>(elapsed).count() >= LOCKOUT_SECS) {
            attempts.erase(it); // lockout expired
            return false;
        }
        return true;
    }

    /// Record a failed login attempt
    void recordFailure(const std::string& username) {
        auto& entry = attempts[username];
        entry.first++;
        entry.second = std::chrono::steady_clock::now();
        int remaining = MAX_ATTEMPTS - entry.first;
        if (remaining > 0)
            std::cout << "[!] Invalid credentials. " << remaining << " attempt(s) remaining.\n";
        else
            std::cout << "[!] Account locked for 15 minutes due to too many failed attempts.\n";
    }

    /// Record a successful login (clear failure history)
    void recordSuccess(const std::string& username) {
        attempts.erase(username);
    }

    /// Strip leading/trailing whitespace and remove control characters
    static std::string sanitize(const std::string& input) {
        std::string s = input;
        // trim
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char c){ return !std::isspace(c); }));
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char c){ return !std::isspace(c); }).base(), s.end());
        // remove non-printable characters
        s.erase(std::remove_if(s.begin(), s.end(), [](unsigned char c){ return c < 32 || c == 127; }), s.end());
        return s;
    }

    /// Validate non-empty after sanitization
    static bool isValid(const std::string& input) {
        return !sanitize(input).empty();
    }

    /// Display welcome banner for logged-in user
    static void showWelcomeBanner(const std::string& name, const std::string& role) {
        std::cout << "\n";
        std::cout << "╔══════════════════════════════════════════╗\n";
        std::cout << "║                                          ║\n";
        // truncate to fit box (inner width 42, prefix "   Hey, " = 8 chars, suffix " ║" = 2)
        std::string displayName = name.size() > 34 ? name.substr(0, 31) + "..." : name;
        std::string displayRole = role.size() > 17 ? role.substr(0, 14) + "..." : role;
        std::cout << "║   Hey, " << displayName;
        int pad = 34 - (int)displayName.size();
        for (int i = 0; i < pad; i++) std::cout << " ";
        std::cout << "║\n";
        std::cout << "║   You are logged in as: " << displayRole;
        int pad2 = 17 - (int)displayRole.size();
        for (int i = 0; i < pad2; i++) std::cout << " ";
        std::cout << "║\n";
        std::cout << "║                                          ║\n";
        std::cout << "╚══════════════════════════════════════════╝\n";
    }
};
