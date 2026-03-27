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
using namespace std;

extern vector<Student>     students;
extern AttendanceManager   attendanceManager;
extern GradeManager        gradeManager;
extern LMSManager          lmsManager;
extern Timetable           timetable;
extern AnnouncementManager announcementManager;
extern ExamManager         examManager;
extern NotificationManager notifManager;

string todayStr();
string getLine(const string& prompt);
void   saveAll();

void teacherMenu(Teacher& teacher) {
    int choice;
    do {
        teacher.viewDashboard();
        cout << "Select: "; cin >> choice;

        if (choice == 1) { // Mark Attendance
            int sid; bool present;
            cout << "Student ID: "; cin >> sid;
            string sname = "Unknown", cls = "";
            for (auto& s : students) if (s.id == sid) { sname = s.name; cls = s.className; }
            string date = getLine("Date (YYYY-MM-DD): ");
            cout << "Present? (1/0): "; cin >> present;
            attendanceManager.mark(sid, sname, cls, date, present);
            if (!present)
                notifManager.send(sid, "Absence recorded on " + date, date, "Attendance");

        } else if (choice == 2) { // Add Grades
            int sid; string subj, term; double marks, total;
            cout << "Student ID: "; cin >> sid;
            string sname = "Unknown";
            for (auto& s : students) if (s.id == sid) sname = s.name;
            subj  = getLine("Subject: ");
            term  = getLine("Term: ");
            cout << "Marks: "; cin >> marks;
            cout << "Total: "; cin >> total;
            gradeManager.addGrade(sid, sname, subj, term, marks, total);

        } else if (choice == 3) { // Upload Assignment
            string title = getLine("Title: ");
            string subj  = getLine("Subject: ");
            string due   = getLine("Due Date: ");
            string file  = getLine("File Reference: ");
            string cls   = getLine("Class: ");
            lmsManager.uploadAssignment(teacher.id, title, subj, due, file, cls);
            notifManager.broadcast("New assignment: " + title + " due " + due, todayStr(), "Assignment");

        } else if (choice == 4) { // Upload Material
            string title = getLine("Title: ");
            string subj  = getLine("Subject: ");
            string path  = getLine("Content Path: ");
            string cls   = getLine("Class: ");
            lmsManager.uploadMaterial(teacher.id, title, subj, path, cls);

        } else if (choice == 5) { // Timetable
            timetable.viewByClass(getLine("Class: "));

        } else if (choice == 6) { // Announcement
            string title   = getLine("Title: ");
            string content = getLine("Content: ");
            string date    = getLine("Date (YYYY-MM-DD): ");
            announcementManager.post(title, content, date, teacher.name, Role::Student, true);
            notifManager.broadcast("New announcement: " + title, date, "Announcement");

        } else if (choice == 7) { // Exams
            cout << "\n1. Schedule Exam  2. Enter Result  3. View Class Results\nSelect: ";
            int c; cin >> c;
            if (c == 1) {
                string title = getLine("Title: ");
                string type  = getLine("Type: ");
                string cls   = getLine("Class: ");
                string subj  = getLine("Subject: ");
                string date  = getLine("Date (YYYY-MM-DD): ");
                double total; cout << "Total Marks: "; cin >> total;
                examManager.scheduleExam(title, type, cls, subj, date, total);
            } else if (c == 2) {
                examManager.listExams();
                int eid, sid; double marks;
                cout << "Exam ID: "; cin >> eid;
                cout << "Student ID: "; cin >> sid;
                string sname = "Unknown";
                for (auto& s : students) if (s.id == sid) sname = s.name;
                cout << "Marks: "; cin >> marks;
                examManager.enterResult(eid, sid, sname, marks);
            } else {
                examManager.viewResultsByClass(getLine("Class: "));
            }

        } else if (choice == 8) { // Analytics
            cout << "\n1. Attendance by Class  2. Grade Report  3. Subject Averages\nSelect: ";
            int c; cin >> c;
            string cls = getLine("Class: ");
            vector<pair<int,string>> sl;
            for (auto& s : students) sl.push_back({s.id, s.className});
            if (c == 1)      Analytics::attendanceReportByClass(attendanceManager, cls);
            else if (c == 2) Analytics::gradeReportByClass(gradeManager, sl, cls);
            else             Analytics::subjectAverageByClass(gradeManager, cls, sl);

        } else if (choice == 9) { // Notifications
            notifManager.viewAll(teacher.id);
        }

        saveAll();
    } while (choice != 10);
}
