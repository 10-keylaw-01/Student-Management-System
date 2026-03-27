#pragma once
#include <iostream>
#include "../Admin.h"
#include "../Attendance.h"
#include "../Grade.h"
#include "../Fee.h"
#include "../Announcement.h"
#include "../Exam.h"
#include "../Notification.h"
#include "../Calendar.h"

extern AttendanceManager   attendanceManager;
extern GradeManager        gradeManager;
extern FeeManager          feeManager;
extern AnnouncementManager announcementManager;
extern ExamManager         examManager;
extern NotificationManager notifManager;
extern AcademicCalendar    calendar;

std::string todayStr();
void saveAll();
int getValidInt(const std::string& prompt, int minVal, int maxVal);
double getValidDouble(const std::string& prompt, double minVal);

void parentMenu(Parent& parent) {
    int choice;
    do {
        parent.viewDashboard();
        choice = getValidInt("Select: ", 1, 8);

        if (choice == 1) {
            attendanceManager.viewByStudent(parent.studentId);
        } else if (choice == 2) {
            gradeManager.viewReport(parent.studentId);
        } else if (choice == 3) {
            feeManager.checkOutstanding(parent.studentId, todayStr());
        } else if (choice == 4) {
            announcementManager.view(Role::Parent);
        } else if (choice == 5) {
            examManager.viewResultsByStudent(parent.studentId);
        } else if (choice == 6) {
            calendar.listUpcoming(todayStr());
        } else if (choice == 7) {
            notifManager.viewAll(parent.id);
        } else if (choice == 8) {
            std::cout << "[✓] Logging out...\n";
            break;
        }
    } while (true);
}
