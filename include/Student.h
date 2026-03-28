#pragma once
#include "User.h"
#include <vector>

/// Attendance record structure
struct AttendanceRecord { 
    std::string date; 
    bool present; 
};

/// Grade record structure
struct GradeRecord { 
    std::string subject, term; 
    double marks, total; 
};

/// Student user class - handles student dashboard and data
class Student : public User {
public:
    std::string rollNumber, className;
    int parentId = 0;

    Student() = default;
    Student(int id, const std::string& username, const std::string& password, 
            const std::string& name, const std::string& roll, const std::string& cls, int parentId)
        : User(id, username, password, name, Role::Student),
          rollNumber(roll), className(cls), parentId(parentId) {}

    void viewDashboard() const override {
        std::cout << "\n=== STUDENT DASHBOARD: " << name << " ===\n"
             << " 1. View Attendance\n"
             << " 2. View Grades\n"
             << " 3. View Assignments\n"
             << " 4. View Materials\n"
             << " 5. View Timetable\n"
             << " 6. View Exam Results\n"
             << " 7. View My Courses\n"
             << " 8. View Fee Status\n"
             << " 9. View Announcements\n"
             << "10. Academic Calendar\n"
             << "11. Notifications\n"
             << "12. Logout\n";
    }
};
