#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include "include/Admin.h"
#include "include/Student.h"
#include "include/Teacher.h"
#include "include/Fee.h"
#include "include/Attendance.h"
#include "include/Grade.h"
#include "include/Timetable.h"
#include "include/Announcement.h"
#include "include/Assignment.h"
#include "include/Book.h"
#include "include/FileManager.h"
using namespace std;

// ── Global state ──────────────────────────────────────────────────────────────
vector<Student>  students;
vector<Teacher>  teachers;
vector<Admin>    admins;
vector<Parent>   parents;
FeeManager       feeManager;
AttendanceManager attendanceManager;
GradeManager     gradeManager;
Timetable        timetable;
AnnouncementManager announcementManager;
LMSManager       lmsManager;
Library          library;

// ── Helpers ───────────────────────────────────────────────────────────────────
int nextId() {
    return (int)(students.size() + teachers.size() + admins.size() + parents.size()) + 1;
}

void saveAll() {
    FileManager::saveStudents(students);
    FileManager::saveTeachers(teachers);
    FileManager::saveAdmins(admins);
    FileManager::saveParents(parents);
    FileManager::saveFees(feeManager.getRecords());
    FileManager::saveAttendance(attendanceManager.getEntries());
    FileManager::saveGrades(gradeManager.getEntries());
    FileManager::saveTimetable(timetable.getSlots());
    FileManager::saveAnnouncements(announcementManager.getAll());
    FileManager::saveAssignments(lmsManager.getAssignments());
    FileManager::saveMaterials(lmsManager.getMaterials());
    FileManager::saveBooks(library.getBooks());
}

void loadAll() {
    students  = FileManager::loadStudents();
    teachers  = FileManager::loadTeachers();
    admins    = FileManager::loadAdmins();
    parents   = FileManager::loadParents();
    FileManager::loadFees(feeManager);
    FileManager::loadAttendance(attendanceManager);
    FileManager::loadGrades(gradeManager);
    FileManager::loadTimetable(timetable);
    FileManager::loadAnnouncements(announcementManager);
    FileManager::loadAssignments(lmsManager);
    FileManager::loadMaterials(lmsManager);
    FileManager::loadBooks(library);
}

void clearInput() { cin.ignore(numeric_limits<streamsize>::max(), '\n'); }

string getLine(const string& prompt) {
    string s; cout << prompt; clearInput(); getline(cin, s); return s;
}

// ── Admin Menu ────────────────────────────────────────────────────────────────
void adminMenu(Admin& admin) {
    int choice;
    do {
        admin.viewDashboard();
        cout << "Select: "; cin >> choice;
        if (choice == 1) { // Manage Students
            cout << "\n1. Add Student  2. List Students\nSelect: "; int c; cin >> c;
            if (c == 1) {
                string uname, pass, name, roll, cls; int pid;
                uname = getLine("Username: "); pass = getLine("Password: ");
                name  = getLine("Name: ");     roll = getLine("Roll No: ");
                cls   = getLine("Class: ");
                cout << "Parent ID (0 if none): "; cin >> pid;
                students.emplace_back(nextId(), uname, pass, name, roll, cls, pid);
                cout << "[Student added.]\n";
            } else {
                if (students.empty()) { cout << "No students.\n"; continue; }
                for (auto& s : students)
                    cout << "ID:" << s.id << " | " << s.name << " | Roll:" << s.rollNumber
                         << " | Class:" << s.className << "\n";
            }
        } else if (choice == 2) { // Manage Teachers
            cout << "\n1. Add Teacher  2. List Teachers\nSelect: "; int c; cin >> c;
            if (c == 1) {
                string uname, pass, name, subj, dept;
                uname = getLine("Username: "); pass = getLine("Password: ");
                name  = getLine("Name: ");     subj = getLine("Subject: ");
                dept  = getLine("Department: ");
                teachers.emplace_back(nextId(), uname, pass, name, subj, dept);
                cout << "[Teacher added.]\n";
            } else {
                if (teachers.empty()) { cout << "No teachers.\n"; continue; }
                for (auto& t : teachers)
                    cout << "ID:" << t.id << " | " << t.name << " | " << t.subject << "\n";
            }
        } else if (choice == 3) { // Fee Management
            cout << "\n1. Generate Invoice  2. Record Payment  3. List All Fees\nSelect: "; int c; cin >> c;
            if (c == 1) {
                int sid; double amt; string due;
                cout << "Student ID: "; cin >> sid;
                string sname = "Unknown";
                for (auto& s : students) if (s.id == sid) sname = s.name;
                cout << "Amount: "; cin >> amt;
                due = getLine("Due Date (YYYY-MM-DD): ");
                feeManager.generateInvoice(sid, sname, amt, due);
            } else if (c == 2) {
                int inv; double amt; string date;
                cout << "Invoice ID: "; cin >> inv;
                cout << "Amount Paid: "; cin >> amt;
                date = getLine("Date (YYYY-MM-DD): ");
                feeManager.recordPayment(inv, amt, date);
            } else {
                feeManager.listAll();
            }
        } else if (choice == 4) { // Library
            cout << "\n1. Add Book  2. Issue Book  3. Return Book  4. List Books\nSelect: "; int c; cin >> c;
            if (c == 1) {
                string title, author;
                title  = getLine("Title: ");
                author = getLine("Author: ");
                library.addBook(title, author);
            } else if (c == 2) {
                int bid, sid; string date;
                cout << "Book ID: "; cin >> bid;
                cout << "Student ID: "; cin >> sid;
                date = getLine("Date (YYYY-MM-DD): ");
                library.issueBook(bid, sid, date);
            } else if (c == 3) {
                int bid; cout << "Book ID: "; cin >> bid;
                library.returnBook(bid);
            } else {
                library.listBooks();
            }
        } else if (choice == 5) { // Timetable
            cout << "\n1. Add Slot  2. View by Class\nSelect: "; int c; cin >> c;
            if (c == 1) {
                string cls, day, start, end, subj, teacher;
                cls     = getLine("Class: ");   day   = getLine("Day: ");
                start   = getLine("Start Time: "); end = getLine("End Time: ");
                subj    = getLine("Subject: "); teacher = getLine("Teacher Name: ");
                timetable.addSlot(cls, day, start, end, subj, teacher);
            } else {
                string cls = getLine("Class: ");
                timetable.viewByClass(cls);
            }
        } else if (choice == 6) { // Announcement
            string title, content, date;
            title   = getLine("Title: ");
            content = getLine("Content: ");
            date    = getLine("Date (YYYY-MM-DD): ");
            announcementManager.post(title, content, date, admin.name, Role::Student, true);
        } else if (choice == 7) { // Reports
            cout << "\nStudents: " << students.size()
                 << " | Teachers: " << teachers.size()
                 << " | Parents: " << parents.size() << "\n";
            cout << "Fee Records:\n"; feeManager.listAll();
            cout << "Books:\n"; library.listBooks();
        }
        saveAll();
    } while (choice != 8);
}

// ── Teacher Menu ──────────────────────────────────────────────────────────────
void teacherMenu(Teacher& teacher) {
    int choice;
    do {
        teacher.viewDashboard();
        cout << "Select: "; cin >> choice;
        if (choice == 1) { // Mark Attendance
            int sid; bool present; string date, cls;
            cout << "Student ID: "; cin >> sid;
            string sname = "Unknown"; cls = "";
            for (auto& s : students) if (s.id == sid) { sname = s.name; cls = s.className; }
            date = getLine("Date (YYYY-MM-DD): ");
            cout << "Present? (1/0): "; cin >> present;
            attendanceManager.mark(sid, sname, cls, date, present);
        } else if (choice == 2) { // Add Grades
            int sid; string subj, term; double marks, total;
            cout << "Student ID: "; cin >> sid;
            string sname = "Unknown";
            for (auto& s : students) if (s.id == sid) sname = s.name;
            subj  = getLine("Subject: "); term = getLine("Term: ");
            cout << "Marks: "; cin >> marks;
            cout << "Total: "; cin >> total;
            gradeManager.addGrade(sid, sname, subj, term, marks, total);
        } else if (choice == 3) { // Upload Assignment
            string title, subj, due, file, cls;
            title = getLine("Title: ");   subj = getLine("Subject: ");
            due   = getLine("Due Date: "); file = getLine("File Reference: ");
            cls   = getLine("Class: ");
            lmsManager.uploadAssignment(teacher.id, title, subj, due, file, cls);
        } else if (choice == 4) { // Upload Material
            string title, subj, path, cls;
            title = getLine("Title: ");   subj = getLine("Subject: ");
            path  = getLine("Content Path: "); cls = getLine("Class: ");
            lmsManager.uploadMaterial(teacher.id, title, subj, path, cls);
        } else if (choice == 5) { // Timetable
            string cls = getLine("Class: ");
            timetable.viewByClass(cls);
        } else if (choice == 6) { // Announcement
            string title, content, date;
            title   = getLine("Title: ");
            content = getLine("Content: ");
            date    = getLine("Date (YYYY-MM-DD): ");
            announcementManager.post(title, content, date, teacher.name, Role::Student, true);
        }
        saveAll();
    } while (choice != 7);
}

// ── Student Menu ──────────────────────────────────────────────────────────────
void studentMenu(Student& student) {
    int choice;
    do {
        student.viewDashboard();
        cout << "Select: "; cin >> choice;
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
        }
    } while (choice != 6);
}

// ── Parent Menu ───────────────────────────────────────────────────────────────
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
            feeManager.checkOutstanding(parent.studentId);
        } else if (choice == 4) {
            announcementManager.view(Role::Parent);
        }
    } while (choice != 5);
}

// ── Login ─────────────────────────────────────────────────────────────────────
bool login() {
    string uname, pass;
    clearInput();  // consume newline left by cin >> choice
    cout << "Username: "; getline(cin, uname);
    cout << "Password: "; getline(cin, pass);

    for (auto& a : admins)
        if (a.username == uname && a.password == pass) {
            cout << "Role: Admin\n"; adminMenu(a); return true;
        }
    for (auto& t : teachers)
        if (t.username == uname && t.password == pass) {
            cout << "Role: Teacher\n"; teacherMenu(t); return true;
        }
    for (auto& s : students)
        if (s.username == uname && s.password == pass) {
            cout << "Role: Student\n"; studentMenu(s); return true;
        }
    for (auto& p : parents)
        if (p.username == uname && p.password == pass) {
            cout << "Role: Parent\n"; parentMenu(p); return true;
        }

    cout << "Invalid credentials.\n";
    return false;
}

// ── Seed default admin if no data ─────────────────────────────────────────────
void seedDefaults() {
    if (admins.empty())
        admins.emplace_back(1, "admin", "admin123", "System Admin");
}

// ── Main ──────────────────────────────────────────────────────────────────────
int main() {
    loadAll();
    seedDefaults();

    int choice;
    do {
        cout << "\n=== SCHOOL MANAGEMENT SYSTEM ===\n"
             << "1. Login\n2. Exit\nSelect: ";
        cin >> choice;
        if (choice == 1) login();
    } while (choice != 2);

    saveAll();
    cout << "Goodbye!\n";
    return 0;
}
