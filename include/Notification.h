#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
using namespace std;

struct Notification {
    int id, userId;       // userId = -1 means broadcast to all
    string message, date, category;
    bool read;
};

class NotificationManager {
    vector<Notification> notifications;
    int nextId = 1;

public:
    // Send to a specific user
    void send(int userId, const string& message, const string& date,
              const string& category = "General") {
        notifications.push_back({nextId++, userId, message, date, category, false});
    }

    // Broadcast to all users
    void broadcast(const string& message, const string& date,
                   const string& category = "General") {
        notifications.push_back({nextId++, -1, message, date, category, false});
    }

    // Show unread notifications for a user, mark them read
    void showAndMarkRead(int userId) {
        bool any = false;
        for (auto& n : notifications) {
            if ((n.userId == userId || n.userId == -1) && !n.read) {
                cout << "  [" << n.category << "] " << n.date << " - " << n.message << "\n";
                n.read = true;
                any = true;
            }
        }
        if (!any) cout << "  No new notifications.\n";
    }

    // View all notifications for a user (read + unread)
    void viewAll(int userId) const {
        bool any = false;
        cout << "\n--- Notifications ---\n";
        for (auto& n : notifications) {
            if (n.userId == userId || n.userId == -1) {
                cout << (n.read ? "  [READ]   " : "  [UNREAD] ")
                     << "[" << n.category << "] " << n.date << " - " << n.message << "\n";
                any = true;
            }
        }
        if (!any) cout << "  No notifications.\n";
    }

    int unreadCount(int userId) const {
        int c = 0;
        for (auto& n : notifications)
            if ((n.userId == userId || n.userId == -1) && !n.read) c++;
        return c;
    }

    vector<Notification>& getAll() { return notifications; }
    void setNextId(int id) { nextId = id; }
};
