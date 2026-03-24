#pragma once
#include "User.h"

class Admin : public User {
public:
    Admin() = default;
    Admin(int id, string username, string password, string name)
        : User(id, username, password, name, Role::Admin) {}

    void viewDashboard() const override {
        cout << "\n=== ADMIN DASHBOARD ===\n"
             << "1. Manage Students\n2. Manage Teachers\n"
             << "3. Fee Management\n4. Library Management\n"
             << "5. Timetable Management\n6. Send Announcement\n"
             << "7. View Reports\n8. Logout\n";
    }
};

class Parent : public User {
public:
    int studentId;

    Parent() = default;
    Parent(int id, string username, string password, string name, int studentId)
        : User(id, username, password, name, Role::Parent), studentId(studentId) {}

    void viewDashboard() const override {
        cout << "\n=== PARENT DASHBOARD: " << name << " ===\n"
             << "1. View Child Attendance\n2. View Child Grades\n"
             << "3. View Fee Status\n4. View Announcements\n5. Logout\n";
    }
};
