#pragma once
#include "User.h"

class Admin : public User {
public:
    Admin() = default;
    Admin(int id, string username, string password, string name)
        : User(id, username, password, name, Role::Admin) {}

    void viewDashboard() const override {
        cout << "\n=== ADMIN DASHBOARD ===\n"
             << " 1. Manage Students\n 2. Manage Teachers\n"
             << " 3. Fee Management\n 4. Library Management\n"
             << " 5. Timetable Management\n 6. Send Announcement\n"
             << " 7. View Reports\n 8. Exam Management\n"
             << " 9. Course Management\n10. Analytics\n"
             << "11. Academic Calendar\n12. Notifications\n"
             << "13. Logout\n";
    }
};

class Parent : public User {
public:
    int studentId;

    Parent() = default;
    Parent(int id, string username, string password, string name, int studentId)
        : User(id, username, password, name, Role::Parent), studentId(studentId) {}

    void viewDashboard() const override {
        cout << "\n=== PARENT DASHBOARD: " << name << " ===\n"
             << "1. View Child Attendance\n2. View Child Grades\n"
             << "3. View Fee Status\n4. View Announcements\n"
             << "5. View Child Exam Results\n6. Academic Calendar\n"
             << "7. Notifications\n8. Logout\n";
    }
};
void adminMenu(Admin& admin) {
    int choice;
    do {
        admin.viewDashboard();
        cout << "Select: "; cin >> choice; clearInput();

        if (choice == 1) { // Manage Students
            cout << "\n1. Add Student  2. List Students\nSelect: "; int c; cin >> c; clearInput();
            if (c == 1) {
                string uname = getLine("Username: ");
                string pass  = getLine("Password: ");
                string name  = getLine("Name: ");
                string roll  = getLine("Roll No: ");
                string cls   = getLine("Class: ");
                int pid; cout << "Parent ID (0 if none): "; cin >> pid;
                students.emplace_back(nextId(), uname, pass, name, roll, cls, pid);
                notifManager.send(students.back().id,
                    "Welcome to the School Management System, " + name + "!", todayStr(), "System");
                cout << "[Student added.]\n";
            } else {
                if (students.empty()) { cout << "No students.\n"; }
                else for (auto& s : students)
                    cout << "ID:" << s.id << " | " << s.name
                         << " | Roll:" << s.rollNumber << " | Class:" << s.className << "\n";
            }

        } else if (choice == 2) { // Manage Teachers
            cout << "\n1. Add Teacher  2. List Teachers\nSelect: "; int c; cin >> c;
            if (c == 1) {
                string uname = getLine("Username: ");
                string pass  = getLine("Password: ");
                string name  = getLine("Name: ");
                string subj  = getLine("Subject: ");
                string dept  = getLine("Department: ");
                teachers.emplace_back(nextId(), uname, pass, name, subj, dept);
                cout << "[Teacher added.]\n";
            } else {
                if (teachers.empty()) { cout << "No teachers.\n"; }
                else for (auto& t : teachers)
                    cout << "ID:" << t.id << " | " << t.name << " | " << t.subject << "\n";
            }

        } else if (choice == 3) { // Fee Management
            cout << "\n1. Generate Invoice  2. Record Payment  3. List All Fees\n"
                 << "4. Set Fee Structure  5. List Fee Structure\n"
                 << "6. Auto-Invoice by Class  7. Print Receipt\nSelect: ";
            int c; cin >> c;
            if (c == 1) {
                int sid; double amt; string due;
                cout << "Student ID: "; cin >> sid;
                string sname = "Unknown", scls = "";
                for (auto& s : students) if (s.id == sid) { sname = s.name; scls = s.className; }
                cout << "Amount: "; cin >> amt;
                due = getLine("Due Date (YYYY-MM-DD): ");
                feeManager.generateInvoice(sid, sname, scls, amt, due);
                notifManager.send(sid, "Fee invoice generated. Amount: " +
                    to_string(amt) + " due " + due, todayStr(), "Fee");
            } else if (c == 2) {
                int inv; double amt;
                cout << "Invoice ID: "; cin >> inv;
                cout << "Amount Paid: "; cin >> amt;
                string date = getLine("Date (YYYY-MM-DD): ");
                feeManager.recordPayment(inv, amt, date);
            } else if (c == 3) {
                feeManager.listAll();
            } else if (c == 4) {
                string cls = getLine("Class: ");
                double fee; cout << "Termly Fee: "; cin >> fee;
                feeManager.setFeeStructure(cls, fee);
            } else if (c == 5) {
                feeManager.listFeeStructure();
            } else if (c == 6) {
                int sid; string due;
                cout << "Student ID: "; cin >> sid;
                string sname = "Unknown", scls = "";
                for (auto& s : students) if (s.id == sid) { sname = s.name; scls = s.className; }
                due = getLine("Due Date (YYYY-MM-DD): ");
                feeManager.generateFromStructure(sid, sname, scls, due);
            } else if (c == 7) {
                int inv; cout << "Invoice ID: "; cin >> inv;
                feeManager.printReceiptById(inv);
            }

        } else if (choice == 4) { // Library
            cout << "\n1. Add Book  2. Issue Book  3. Return Book  4. List Books\nSelect: ";
            int c; cin >> c;
            if (c == 1) {
                library.addBook(getLine("Title: "), getLine("Author: "));
            } else if (c == 2) {
                int bid, sid; cout << "Book ID: "; cin >> bid;
                cout << "Student ID: "; cin >> sid;
                library.issueBook(bid, sid, getLine("Date (YYYY-MM-DD): "));
            } else if (c == 3) {
                int bid; cout << "Book ID: "; cin >> bid; library.returnBook(bid);
            } else {
                library.listBooks();
            }

        } else if (choice == 5) { // Timetable
            cout << "\n1. Add Slot  2. View by Class\nSelect: "; int c; cin >> c;
            if (c == 1) {
                string cls   = getLine("Class: ");
                string day   = getLine("Day: ");
                string start = getLine("Start Time: ");
                string end   = getLine("End Time: ");
                string subj  = getLine("Subject: ");
                string tname = getLine("Teacher Name: ");
                timetable.addSlot(cls, day, start, end, subj, tname);
            } else {
                timetable.viewByClass(getLine("Class: "));
            }

        } else if (choice == 6) { // Announcement
            string title   = getLine("Title: ");
            string content = getLine("Content: ");
            string date    = getLine("Date (YYYY-MM-DD): ");
            announcementManager.post(title, content, date, admin.name, Role::Student, true);
            notifManager.broadcast("New announcement: " + title, date, "Announcement");

        } else if (choice == 7) { // Reports
            cout << "\nStudents: " << students.size()
                 << " | Teachers: " << teachers.size()
                 << " | Parents: " << parents.size() << "\n";
            cout << "Fee Records:\n"; feeManager.listAll();
            cout << "Books:\n"; library.listBooks();

        } else if (choice == 8) { // Exams
            cout << "\n1. Schedule Exam  2. Enter Result  3. List Exams  4. Class Results\nSelect: ";
            int c; cin >> c;
            if (c == 1) {
                string title = getLine("Title: ");
                string type  = getLine("Type (Midterm/Final/Quiz/Practical): ");
                string cls   = getLine("Class: ");
                string subj  = getLine("Subject: ");
                string date  = getLine("Date (YYYY-MM-DD): ");
                double total; cout << "Total Marks: "; cin >> total;
                examManager.scheduleExam(title, type, cls, subj, date, total);
                notifManager.broadcast("Exam scheduled: " + title + " on " + date, date, "Exam");
            } else if (c == 2) {
                int eid, sid; double marks;
                examManager.listExams();
                cout << "Exam ID: "; cin >> eid;
                cout << "Student ID: "; cin >> sid;
                string sname = "Unknown";
                for (auto& s : students) if (s.id == sid) sname = s.name;
                cout << "Marks: "; cin >> marks;
                examManager.enterResult(eid, sid, sname, marks);
            } else if (c == 3) {
                examManager.listExams();
            } else {
                string cls = getLine("Class: ");
                examManager.viewResultsByClass(cls);
            }

        } else if (choice == 9) { // Courses
            cout << "\n1. Add Course  2. Modify Course  3. Delete Course  4. List All\nSelect: ";
            int c; cin >> c;
            if (c == 1) {
                string code  = getLine("Code: ");
                string name  = getLine("Name: ");
                string subj  = getLine("Subject: ");
                string cls   = getLine("Class: ");
                string tname = getLine("Teacher Name: ");
                int cr; cout << "Credits: "; cin >> cr;
                courseManager.addCourse(code, name, subj, cls, tname, cr);
            } else if (c == 2) {
                int id; cout << "Course ID: "; cin >> id;
                string name  = getLine("New Name (Enter to skip): ");
                string tname = getLine("New Teacher (Enter to skip): ");
                int cr; cout << "New Credits (0 to skip): "; cin >> cr;
                courseManager.modifyCourse(id, name, tname, cr);
            } else if (c == 3) {
                int id; cout << "Course ID: "; cin >> id;
                courseManager.deleteCourse(id);
            } else {
                courseManager.listAll();
            }

        } else if (choice == 10) { // Analytics
            cout << "\n1. Attendance by Class  2. Attendance Summary\n"
                 << "3. Grade Report by Class  4. Subject Averages\nSelect: ";
            int c; cin >> c;
            if (c == 1) {
                Analytics::attendanceReportByClass(attendanceManager, getLine("Class: "));
            } else if (c == 2) {
                Analytics::attendanceSummaryAllClasses(attendanceManager);
            } else if (c == 3) {
                string cls = getLine("Class: ");
                vector<pair<int,string>> sl;
                for (auto& s : students) sl.push_back({s.id, s.className});
                Analytics::gradeReportByClass(gradeManager, sl, cls);
            } else {
                string cls = getLine("Class: ");
                vector<pair<int,string>> sl;
                for (auto& s : students) sl.push_back({s.id, s.className});
                Analytics::subjectAverageByClass(gradeManager, cls, sl);
            }

        } else if (choice == 11) { // Calendar
            cout << "\n1. Add Event  2. Remove Event  3. List All  4. Upcoming Events\nSelect: ";
            int c; cin >> c;
            if (c == 1) {
                string title = getLine("Title: ");
                string date  = getLine("Start Date (YYYY-MM-DD): ");
                string end   = getLine("End Date (YYYY-MM-DD): ");
                string type  = getLine("Type (Holiday/Exam/Meeting/Sports/Academic/Other): ");
                string desc  = getLine("Description: ");
                calendar.addEvent(title, date, end, type, desc);
            } else if (c == 2) {
                calendar.listAll();
                int id; cout << "Event ID to remove: "; cin >> id;
                calendar.removeEvent(id);
            } else if (c == 3) {
                calendar.listAll();
            } else {
                calendar.listUpcoming(todayStr());
            }

        } else if (choice == 12) { // Notifications
            notifManager.viewAll(-1); // admin sees broadcasts
        }

        saveAll();
    } while (choice != 13);
}