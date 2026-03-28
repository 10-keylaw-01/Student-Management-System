#pragma once
#include <iostream>
#include <vector>
#include <string>
#include "../Teacher.h"
#include "../Student.h"
#include "../Attendance.h"
#include "../Grade.h"
#include "../Assignment.h"
#include "../Timetable.h"
#include "../Announcement.h"
#include "../Exam.h"
#include "../Notification.h"
#include "../Analytics.h"

extern std::vector<Student>     students;
extern AttendanceManager   attendanceManager;
extern GradeManager        gradeManager;
extern LMSManager          lmsManager;
extern Timetable           timetable;
extern AnnouncementManager announcementManager;
extern ExamManager         examManager;
extern NotificationManager notifManager;

std::string todayStr();
std::string getLine(const std::string& prompt);
std::string getValidDate(const std::string& prompt);
void   saveAll();
int getValidInt(const std::string& prompt, int minVal = INT_MIN, int maxVal = INT_MAX);
double getValidDouble(const std::string& prompt, double minVal = 0.0);

void teacherMenu(const Teacher& teacher) {
    int choice;
    do {
        teacher.viewDashboard();
        choice = getValidInt("Select: ", 1, 11);

        if (choice == 1) { // Mark Attendance
            int sid; bool present;
            sid = getValidInt("Student ID: ", 1, INT_MAX);
            std::string sname = "Unknown", cls = "";
            for (const auto& s : students) if (s.id == sid) { sname = s.name; cls = s.className; }
            
            // Check student found AND authorization
            if (cls.empty()) {
                std::cout << "[!] Student not found.\n";
                continue;
            }
            if (!teacher.isAssignedToClass(cls)) {
                std::cout << "[!] You are not assigned to teach class " << cls << "\n";
                continue;
            }
            
            std::string date = getValidDate("Date (YYYY-MM-DD): ");
            present = getValidInt("Present? (1=Yes, 0=No): ", 0, 1);
            attendanceManager.mark(sid, sname, cls, date, (bool)present);
            if (!present)
                notifManager.send(sid, "Absence recorded on " + date, date, "Attendance");
            std::cout << "[✓] Attendance marked.\n";

        } else if (choice == 2) { // Add Grades
            int sid; std::string subj, term; double marks, total;
            sid = getValidInt("Student ID: ", 1, INT_MAX);
            std::string sname = "Unknown", cls = "";
            for (const auto& s : students) if (s.id == sid) { sname = s.name; cls = s.className; }
            
            // Check student found AND authorization
            if (cls.empty()) {
                std::cout << "[!] Student not found.\n";
                continue;
            }
            if (!teacher.isAssignedToClass(cls)) {
                std::cout << "[!] You are not assigned to teach class " << cls << "\n";
                continue;
            }
            
            subj  = getLine("Subject: ");
            term  = getLine("Term: ");
            marks = getValidDouble("Marks: ");
            total = getValidDouble("Total: ", 1.0);
            gradeManager.addGrade(sid, sname, subj, term, marks, total);
            std::cout << "[✓] Grade added.\n";

        } else if (choice == 3) { // Upload Assignment
            std::string title = getLine("Title: ");
            std::string subj  = getLine("Subject: ");
            std::string due   = getLine("Due Date: ");
            std::string file  = getLine("File Reference: ");
            std::string cls   = getLine("Class: ");
            
            if (!teacher.isAssignedToClass(cls)) {
                std::cout << "[!] You are not assigned to teach class " << cls << "\n";
                continue;
            }
            
            lmsManager.uploadAssignment(teacher.id, title, subj, due, file, cls);
            notifManager.broadcast("New assignment: " + title + " due " + due, todayStr(), "Assignment");
            std::cout << "[✓] Assignment uploaded.\n";

        } else if (choice == 4) { // Upload Material
            std::string title = getLine("Title: ");
            std::string subj  = getLine("Subject: ");
            std::string path  = getLine("Content Path: ");
            std::string cls   = getLine("Class: ");
            
            if (!teacher.isAssignedToClass(cls)) {
                std::cout << "[!] You are not assigned to teach class " << cls << "\n";
                continue;
            }
            
            lmsManager.uploadMaterial(teacher.id, title, subj, path, cls);
            std::cout << "[✓] Material uploaded.\n";

        } else if (choice == 5) { // Timetable
            timetable.viewByClass(getLine("Class: "));

        } else if (choice == 6) { // Announcement
            std::string title   = getLine("Title: ");
            std::string content = getLine("Content: ");
            std::string date    = getValidDate("Date (YYYY-MM-DD): ");
            announcementManager.post(title, content, date, teacher.name, Role::Student, true);
            notifManager.broadcast("New announcement: " + title, date, "Announcement");
            std::cout << "[✓] Announcement posted.\n";

        } else if (choice == 7) { // Exam Management
            std::cout << "\n1. Schedule Exam  2. Enter Result  3. View Class Results\nSelect: ";
            int c = getValidInt("", 1, 3);
            if (c == 1) {
                std::string title = getLine("Title: ");
                std::string type  = getLine("Type: ");
                std::string cls   = getLine("Class: ");
                std::string subj  = getLine("Subject: ");
                std::string date  = getValidDate("Date (YYYY-MM-DD): ");
                double total = getValidDouble("Total Marks: ", 1.0);
                examManager.scheduleExam(title, type, cls, subj, date, total);
                std::cout << "[✓] Exam scheduled.\n";
            } else if (c == 2) {
                examManager.listExams();
                int eid = getValidInt("Exam ID: ", 1, INT_MAX);
                int sid = getValidInt("Student ID: ", 1, INT_MAX);
                std::string sname = "Unknown";
                for (const auto& s : students) if (s.id == sid) sname = s.name;
                double marks = getValidDouble("Marks: ");
                examManager.enterResult(eid, sid, sname, marks);
                std::cout << "[✓] Result entered.\n";
            } else {
                examManager.viewResultsByClass(getLine("Class: "));
            }

        } else if (choice == 8) { // View Class Attendance
            std::string cls = getLine("Class: ");
            attendanceManager.viewByClass(cls, "");
            
        } else if (choice == 9) { // Analytics
            std::cout << "\n1. Attendance by Class  2. Grade Report  3. Subject Averages\nSelect: ";
            int c = getValidInt("", 1, 3);
            std::string cls = getLine("Class: ");
            std::vector<std::pair<int,std::string>> sl;
            for (const auto& s : students) sl.push_back({s.id, s.className});
            if (c == 1)      Analytics::attendanceReportByClass(attendanceManager, cls);
            else if (c == 2) Analytics::gradeReportByClass(gradeManager, sl, cls);
            else             Analytics::subjectAverageByClass(gradeManager, cls, sl);

        } else if (choice == 10) { // Notifications
            notifManager.viewAll(teacher.id);
            
        } else if (choice == 11) { // Logout
            std::cout << "[✓] Logging out...\n";
            break;
        }

    } while (true);
}
