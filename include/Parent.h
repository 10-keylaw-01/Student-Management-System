#pragma once
#include "User.h"
#include <vector>
#include <algorithm>

class Parent : public User {
public:
    int studentId = 0;  // For compatibility with FileManager
    std::vector<int> childrenIds;  // For multiple children support

    Parent() = default;
    Parent(int id, const std::string& username, const std::string& password, 
           const std::string& name, int studentId = 0)
        : User(id, username, password, name, Role::Parent), studentId(studentId) {
        if (studentId > 0) childrenIds.push_back(studentId);
    }

    void viewDashboard() const override {
        std::cout << "\n╔══════════════════════════════════════════╗\n"
                  << "║            PARENT DASHBOARD              ║\n"
                  << "╚══════════════════════════════════════════╝\n"
                  << "Welcome, " << name << "!\n\n"
                  << "1. View Children's Grades\n"
                  << "2. View Children's Attendance\n"
                  << "3. View Fee Status\n"
                  << "4. View Announcements\n"
                  << "5. View Timetable\n"
                  << "6. View Notifications\n"
                  << "7. Change Password\n"
                  << "8. Logout\n\n";
    }

    void addChild(int childId) {
        if (childId > 0 && std::find(childrenIds.begin(), childrenIds.end(), childId) == childrenIds.end()) {
            childrenIds.push_back(childId);
            if (studentId == 0) studentId = childId;  // Set primary child for compatibility
        }
    }
};