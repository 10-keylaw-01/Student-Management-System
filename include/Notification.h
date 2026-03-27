#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>

/// Notification structure
struct Notification {
    int id, userId;       // userId = -1 means broadcast to all
    std::string message, date, category;
    bool read;
};

/// Manages system notifications
class NotificationManager {
    std::vector<Notification> notifications;
    int nextId = 1;

public:
    /// Send notification to a specific user
    void send(int userId, const std::string& message, const std::string& date,
              const std::string& category = "General") {
        notifications.push_back({nextId++, userId, message, date, category, false});
    }

    /// Broadcast notification to all users
    void broadcast(const std::string& message, const std::string& date,
                   const std::string& category = "General") {
        notifications.push_back({nextId++, -1, message, date, category, false});
    }

    /// Show unread notifications for a user, mark them as read
    void showAndMarkRead(int userId) {
        bool any = false;
        for (auto& n : notifications) {
            if ((n.userId == userId || n.userId == -1) && !n.read) {
                std::cout << "  [" << n.category << "] " << n.date << " - " << n.message << "\n";
                n.read = true;
                any = true;
            }
        }
        if (!any) std::cout << "  No new notifications.\n";
    }

    /// View all notifications for a user (read + unread)
    void viewAll(int userId) const {
        bool any = false;
        std::cout << "\n--- Notifications ---\n";
        for (const auto& n : notifications) {
            if (n.userId == userId || n.userId == -1) {
                std::cout << (n.read ? "  [READ]   " : "  [UNREAD] ")
                     << "[" << n.category << "] " << n.date << " - " << n.message << "\n";
                any = true;
            }
        }
        if (!any) std::cout << "  No notifications.\n";
    }

    /// Get count of unread notifications for a user
    int unreadCount(int userId) const {
        int c = 0;
        for (const auto& n : notifications)
            if ((n.userId == userId || n.userId == -1) && !n.read) c++;
        return c;
    }

    std::vector<Notification>& getAll() { return notifications; }
    void setNextId(int id) { nextId = id; }
};
