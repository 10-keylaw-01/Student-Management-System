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
using namespace std;

extern AttendanceManager   attendanceManager;
extern GradeManager        gradeManager;
extern FeeManager          feeManager;
extern AnnouncementManager announcementManager;
extern ExamManager         examManager;
extern NotificationManager notifManager;
extern AcademicCalendar    calendar;

string todayStr();

void parentMenu(Parent& parent) {
    int choice;
    do {
        parent.viewDashboard();
        cout << "Select: "; cin >> choice;

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
        }
    } while (choice != 8);
}
