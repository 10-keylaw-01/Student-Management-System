#pragma once
#include "User.h"
#include <vector>
#include <iostream>

/// Teacher user class - handles teacher functions
class Teacher : public User {
public:
    std::string subject, department;
    std::vector<std::string> assignedClasses;

    Teacher() = default;
    Teacher(int id, const std::string& username, const std::string& password, 
            const std::string& name, const std::string& subject, const std::string& dept)
        : User(id, username, password, name, Role::Teacher),
          subject(subject), department(dept) {}

    void viewDashboard() const override {
        std::cout << "\n=== TEACHER DASHBOARD: " << name << " ===\n"
             << "1. Mark Attendance\n"
             << "2. Add Grades\n"
             << "3. Upload Assignment\n"
             << "4. Upload Material\n"
             << "5. View Timetable\n"
             << "6. Send Announcement\n"
             << "7. Exam Management\n"
             << "8. View Class Attendance\n"
             << "9. Analytics\n"
             << "10. Notifications\n"
             << "11. Logout\n";
    }

    /// Add a class to teacher's assigned classes
    void assignClass(const std::string& cls) { 
        assignedClasses.push_back(cls); 
    }
    
    /// Check if teacher is assigned to this class
    bool isAssignedToClass(const std::string& cls) const {
        for (const auto& c : assignedClasses) if (c == cls) return true;
        return false;
    }
};
