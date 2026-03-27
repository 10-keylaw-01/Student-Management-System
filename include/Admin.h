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

/// Parent user class - tracks parent of a student
class Parent : public User {
public:
    int studentId;

    Parent() = default;
    Parent(int id, const std::string& username, const std::string& password, 
           const std::string& name, int studentId)
        : User(id, username, password, name, Role::Parent), studentId(studentId) {}

    void viewDashboard() const override {
        std::cout << "\n=== PARENT DASHBOARD: " << name << " ===\n"
             << "1. View Child Attendance\n"
             << "2. View Child Grades\n"
             << "3. View Fee Status\n"
             << "4. View Announcements\n"
             << "5. View Child Exam Results\n"
             << "6. Academic Calendar\n"
             << "7. Notifications\n"
             << "8. Logout\n";
    }
};