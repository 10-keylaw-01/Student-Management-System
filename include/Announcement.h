#pragma once
#include <string>
#include <vector>
#include <iostream>
#include "User.h"

/// Announcement structure
struct Announcement {
    int id;
    std::string title, content, date, author;
    Role targetRole;
    bool allRoles;
};

/// Manages announcements for all users
class AnnouncementManager {
    std::vector<Announcement> announcements;
    int nextId = 1;
public:
    void setNextId(int id) { nextId = id; }
    
    /// Post an announcement to specific role or all users
    void post(const std::string& title, const std::string& content, const std::string& date,
              const std::string& author, Role target, bool allRoles = false) {
        announcements.push_back({nextId++, title, content, date, author, target, allRoles});
        std::cout << "[Announcement posted: \"" << title << "\" by " << author << "]\n";
        // Simulate bulk notification
        std::cout << "[Notification sent to " << (allRoles ? "All Users" : "target role") << "]\n";
    }

    /// View announcements for a specific role
    void view(Role role) const {
        bool found = false;
        for (const auto& a : announcements) {
            if (a.allRoles || a.targetRole == role) {
                std::cout << "[" << a.date << "] " << a.title << " (by " << a.author << ")\n"
                     << "  " << a.content << "\n";
                found = true;
            }
        }
        if (!found) std::cout << "No announcements.\n";
    }

    /// View all announcements regardless of target role
    void viewAll() const {
        if (announcements.empty()) {
            std::cout << "No announcements.\n";
            return;
        }
        std::cout << "\n=== All Announcements ===\n";
        for (const auto& a : announcements) {
            std::cout << "[" << a.date << "] " << a.title << " (by " << a.author << ")\n"
                 << "  " << a.content << "\n";
        }
    }

    std::vector<Announcement>& getAll() { return announcements; }
};
