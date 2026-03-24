#pragma once
#include <string>
#include <vector>
#include <iostream>
#include "User.h"
using namespace std;

struct Announcement {
    int id;
    string title, content, date, author;
    Role targetRole;
    bool allRoles;
};

class AnnouncementManager {
    vector<Announcement> announcements;
    int nextId = 1;
public:
    void post(const string& title, const string& content, const string& date,
              const string& author, Role target, bool allRoles = false) {
        announcements.push_back({nextId++, title, content, date, author, target, allRoles});
        cout << "[Announcement posted: \"" << title << "\" by " << author << "]\n";
        // Simulate bulk notification
        cout << "[Notification sent to " << (allRoles ? "All Users" : "target role") << "]\n";
    }

    void view(Role role) const {
        bool found = false;
        for (auto& a : announcements) {
            if (a.allRoles || a.targetRole == role) {
                cout << "[" << a.date << "] " << a.title << " (by " << a.author << ")\n"
                     << "  " << a.content << "\n";
                found = true;
            }
        }
        if (!found) cout << "No announcements.\n";
    }

    vector<Announcement>& getAll() { return announcements; }
};
