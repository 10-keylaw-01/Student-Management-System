#pragma once
#include "User.h"
#include <iostream>
#include <vector>
#include <map>

struct AttendanceRecord { string date; bool present; };
struct GradeRecord { string subject, term; double marks, total; };

class Student : public User {
public:
    string rollNumber, className;
    int parentId;
    vector<AttendanceRecord> attendance;
    vector<GradeRecord> grades;

    Student() = default;
    Student(int id, string username, string password, string name,
            string roll, string cls, int parentId)
        : User(id, username, password, name, Role::Student),
          rollNumber(roll), className(cls), parentId(parentId) {}

    void viewDashboard() const override {
        cout << "\n=== STUDENT DASHBOARD: " << name << " ===\n"
             << " 1. View Attendance\n 2. View Grades\n"
             << " 3. View Assignments\n 4. View Materials\n"
             << " 5. View Timetable\n 6. View Exam Results\n"
             << " 7. View My Courses\n 8. View Fee Status\n"
             << " 9. View Announcements\n10. Academic Calendar\n"
             << "11. Notifications\n12. Logout\n";
    }

    void updateAttendance(const string& date, bool present) {
        attendance.push_back({date, present});
    }

    void addGrade(const string& subject, const string& term, double marks, double total) {
        grades.push_back({subject, term, marks, total});
    }

    double attendancePercent() const {
        if (attendance.empty()) return 0;
        int present = 0;
        for (auto& a : attendance) if (a.present) present++;
        return (present * 100.0) / attendance.size();
    }
};
