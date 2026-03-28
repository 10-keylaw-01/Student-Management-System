#pragma once
#include <iostream>
#include "../Student.h"
#include "../Attendance.h"
#include "../Grade.h"
#include "../Assignment.h"
#include "../Timetable.h"
#include "../Exam.h"
#include "../Course.h"
#include "../Fee.h"
#include "../Announcement.h"
#include "../Notification.h"
#include "../Calendar.h"

extern AttendanceManager   attendanceManager;
extern GradeManager        gradeManager;
extern LMSManager          lmsManager;
extern Timetable           timetable;
extern ExamManager         examManager;
extern CourseManager       courseManager;
extern FeeManager          feeManager;
extern AnnouncementManager announcementManager;
extern NotificationManager notifManager;
extern AcademicCalendar    calendar;

std::string todayStr();
void saveAll();
void studentMenu(const Student& student) {
    int choice;
    do {
        student.viewDashboard();
        choice = getValidInt("Select: ", 1, 12);

        if (choice == 1) {
            attendanceManager.viewByStudent(student.id);
        } else if (choice == 2) {
            gradeManager.viewReport(student.id);
        } else if (choice == 3) {
            lmsManager.viewAssignments(student.className);
        } else if (choice == 4) {
            lmsManager.viewMaterials(student.className);
        } else if (choice == 5) {
            timetable.viewByClass(student.className);
        } else if (choice == 6) {
            examManager.viewResultsByStudent(student.id);
        } else if (choice == 7) {
            courseManager.listByClass(student.className);
        } else if (choice == 8) {
            feeManager.checkOutstanding(student.id, todayStr());
        } else if (choice == 9) {
            announcementManager.view(Role::Student);
        } else if (choice == 10) {
            calendar.listUpcoming(todayStr());
        } else if (choice == 11) {
            notifManager.viewAll(student.id);
        } else if (choice == 12) {
            std::cout << "[✓] Logging out...\n";
            break;
        }
    } while (true);
}
