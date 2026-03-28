#pragma once
#include "User.h"
#include <iostream>

/// Admin user class
class Admin : public User {
public:
    Admin() = default;
    Admin(int id, const std::string& username, const std::string& password, const std::string& name)
        : User(id, username, password, name, Role::Admin) {}

    void viewDashboard() const override {
        std::cout << "\n=== ADMIN DASHBOARD ===\n"
             << " 1. Manage Students\n"
             << " 2. Manage Teachers\n"
             << " 3. Fee Management\n"
             << " 4. Library Management\n"
             << " 5. Timetable Management\n"
             << " 6. Send Announcement\n"
             << " 7. View Reports\n"
             << " 8. Exam Management\n"
             << " 9. Course Management\n"
             << "10. Analytics\n"
             << "11. Academic Calendar\n"
             << "12. Notifications\n"
             << "13. Change Password\n"
             << "14. Logout\n";
    }
};