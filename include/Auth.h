#pragma once
#include <string>
#include <map>
#include <chrono>
#include <algorithm>
#include <iostream>
using namespace std;

class Auth {
    // username -> {attempt_count, lockout_time_point}
    map<string, pair<int, chrono::steady_clock::time_point>> attempts;
    static const int MAX_ATTEMPTS  = 3;
    static const int LOCKOUT_SECS  = 900; // 15 minutes

public:
    // Returns true if the username is currently locked out
    bool isLockedOut(const string& username) {
        auto it = attempts.find(username);
        if (it == attempts.end()) return false;
        if (it->second.first < MAX_ATTEMPTS) return false;
        auto elapsed = chrono::steady_clock::now() - it->second.second;
        if (chrono::duration_cast<chrono::seconds>(elapsed).count() >= LOCKOUT_SECS) {
            attempts.erase(it); // lockout expired
            return false;
        }
        return true;
    }

    void recordFailure(const string& username) {
        auto& entry = attempts[username];
        entry.first++;
        entry.second = chrono::steady_clock::now();
        int remaining = MAX_ATTEMPTS - entry.first;
        if (remaining > 0)
            cout << "[!] Invalid credentials. " << remaining << " attempt(s) remaining.\n";
        else
            cout << "[!] Account locked for 15 minutes due to too many failed attempts.\n";
    }

    void recordSuccess(const string& username) {
        attempts.erase(username);
    }

    // Strip leading/trailing whitespace and remove control characters
    static string sanitize(const string& input) {
        string s = input;
        // trim
        s.erase(s.begin(), find_if(s.begin(), s.end(), [](unsigned char c){ return !isspace(c); }));
        s.erase(find_if(s.rbegin(), s.rend(), [](unsigned char c){ return !isspace(c); }).base(), s.end());
        // remove non-printable characters
        s.erase(remove_if(s.begin(), s.end(), [](unsigned char c){ return c < 32 || c == 127; }), s.end());
        return s;
    }

    // Validate non-empty after sanitization
    static bool isValid(const string& input) {
        return !sanitize(input).empty();
    }

    static void showWelcomeBanner(const string& name, const string& role) {
        cout << "\n";
        cout << "╔══════════════════════════════════════════╗\n";
        cout << "║                                          ║\n";
        // truncate to fit box (inner width 42, prefix "   Hey, " = 8 chars, suffix " ║" = 2)
        string displayName = name.size() > 34 ? name.substr(0, 31) + "..." : name;
        string displayRole = role.size() > 17 ? role.substr(0, 14) + "..." : role;
        cout << "║   Hey, " << displayName;
        int pad = 34 - (int)displayName.size();
        for (int i = 0; i < pad; i++) cout << " ";
        cout << "║\n";
        cout << "║   You are logged in as: " << displayRole;
        int pad2 = 17 - (int)displayRole.size();
        for (int i = 0; i < pad2; i++) cout << " ";
        cout << "║\n";
        cout << "║                                          ║\n";
        cout << "╚══════════════════════════════════════════╝\n";
    }
};
