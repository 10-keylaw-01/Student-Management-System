#pragma once
#include <iostream>
#include "../Parent.h"
#include "../Student.h"
#include "../Attendance.h"
#include "../Grade.h"
#include "../Fee.h"
#include "../Announcement.h"
#include "../Exam.h"
#include "../Notification.h"
#include "../Calendar.h"

extern std::vector<Student> students;
extern AttendanceManager   attendanceManager;
extern GradeManager        gradeManager;
extern FeeManager          feeManager;
extern AnnouncementManager announcementManager;
extern ExamManager         examManager;
extern NotificationManager notifManager;
extern AcademicCalendar    calendar;

std::string todayStr();
std::string getLine(const std::string& prompt);
void saveAll();
void parentMenu(Parent& parent) {
    int choice;
    do {
        parent.viewDashboard();
        choice = getValidInt("Select: ", 1, 8);

        if (choice == 1) {
            // View Children's Grades
            if (parent.childrenIds.empty() && parent.studentId > 0) {
                parent.addChild(parent.studentId);  // Ensure compatibility
            }
            for (int childId : parent.childrenIds) {
                std::cout << "\n=== Grades for Child ID: " << childId << " ===\n";
                gradeManager.viewReport(childId);
            }
        } else if (choice == 2) {
            // View Children's Attendance
            if (parent.childrenIds.empty() && parent.studentId > 0) {
                parent.addChild(parent.studentId);
            }
            for (int childId : parent.childrenIds) {
                std::cout << "\n=== Attendance for Child ID: " << childId << " ===\n";
                attendanceManager.viewByStudent(childId);
            }
        } else if (choice == 3) {
            // View Fee Status
            if (parent.childrenIds.empty() && parent.studentId > 0) {
                parent.addChild(parent.studentId);
            }
            for (int childId : parent.childrenIds) {
                std::cout << "\n=== Fee Status for Child ID: " << childId << " ===\n";
                feeManager.checkOutstanding(childId, todayStr());
            }
        } else if (choice == 4) {
            // View Announcements
            announcementManager.view(Role::Parent);
        } else if (choice == 5) {
            // View Timetable
            if (parent.childrenIds.empty() && parent.studentId > 0) {
                parent.addChild(parent.studentId);
            }
            if (!parent.childrenIds.empty()) {
                int childId = parent.childrenIds[0];
                for (const auto& s : students) {
                    if (s.id == childId) {
                        std::cout << "\n=== Timetable for " << s.name << " (Class: " << s.className << ") ===\n";
                        // Note: Assuming timetable has a viewByClass method
                        // timetable.viewByClass(s.className);
                        std::cout << "Timetable view not fully implemented.\n";
                        break;
                    }
                }
            }
        } else if (choice == 6) {
            // View Notifications
            notifManager.viewAll(parent.id);
        } else if (choice == 7) {
            // Change Password (placeholder)
            std::cout << "Password change functionality not implemented yet.\n";
        } else if (choice == 8) {
            std::cout << "[✓] Logging out...\n";
            break;
        }
    } while (true);
}
