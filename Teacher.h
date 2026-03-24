#pragma once
#include "User.h"
#include <vector>

class Teacher : public User {
public:
    string subject, department;
    vector<string> assignedClasses;

    Teacher() = default;
    Teacher(int id, string username, string password, string name,
            string subject, string dept)
        : User(id, username, password, name, Role::Teacher),
          subject(subject), department(dept) {}

    void viewDashboard() const override {
        cout << "\n=== TEACHER DASHBOARD: " << name << " ===\n"
             << "1. Mark Attendance\n2. Add Grades\n"
             << "3. Upload Assignment\n4. Upload Material\n"
             << "5. View Timetable\n6. Send Announcement\n7. Logout\n";
    }

    void assignClass(const string& cls) { assignedClasses.push_back(cls); }
};
