#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include <ctime>
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
#include "include/Exam.h"
#include "include/Course.h"
#include "include/Notification.h"
#include "include/Calendar.h"
#include "include/Analytics.h"
#include "include/Auth.h"
#include "include/FileManager.h"
#include "include/menus/TeacherMenu.h"
#include "include/menus/StudentMenu.h"
#include "include/menus/ParentMenu.h"
using namespace std;

// ── Global state ──────────────────────────────────────────────────────────────
vector<Student>     students;
vector<Teacher>     teachers;
vector<Admin>       admins;
vector<Parent>      parents;
FeeManager          feeManager;
AttendanceManager   attendanceManager;
GradeManager        gradeManager;
Timetable           timetable;
AnnouncementManager announcementManager;
LMSManager          lmsManager;
Library             library;
ExamManager         examManager;
CourseManager       courseManager;
NotificationManager notifManager;
AcademicCalendar    calendar;
Auth                auth;

// ── Helpers ───────────────────────────────────────────────────────────────────
static int globalMaxId = 0;

int nextId() {
    return ++globalMaxId;
}

string todayStr() {
    time_t t = time(nullptr);
    tm* tm_ = localtime(&t);
    char buf[11];
    strftime(buf, sizeof(buf), "%Y-%m-%d", tm_);
    return string(buf);
}

void saveAll() {
    FileManager::saveStudents(students);
    FileManager::saveTeachers(teachers);
    FileManager::saveAdmins(admins);
    FileManager::saveParents(parents);
    FileManager::saveFees(feeManager.getRecords());
    FileManager::saveFeeStructure(feeManager.getStructures());
    FileManager::saveAttendance(attendanceManager.getEntries());
    FileManager::saveGrades(gradeManager.getEntries());
    FileManager::saveTimetable(timetable.getSlots());
    FileManager::saveAnnouncements(announcementManager.getAll());
    FileManager::saveAssignments(lmsManager.getAssignments());
    FileManager::saveMaterials(lmsManager.getMaterials());
    FileManager::saveBooks(library.getBooks());
    FileManager::saveExams(examManager.getExams());
    FileManager::saveExamResults(examManager.getResults());
    FileManager::saveCourses(courseManager.getCourses());
    FileManager::saveNotifications(notifManager.getAll());
    FileManager::saveCalendar(calendar.getEvents());
}

void loadAll() {
    students  = FileManager::loadStudents();
    teachers  = FileManager::loadTeachers();
    admins    = FileManager::loadAdmins();
    parents   = FileManager::loadParents();
    FileManager::loadFees(feeManager);
    FileManager::loadFeeStructure(feeManager);
    FileManager::loadAttendance(attendanceManager);
    FileManager::loadGrades(gradeManager);
    FileManager::loadTimetable(timetable);
    FileManager::loadAnnouncements(announcementManager);
    FileManager::loadAssignments(lmsManager);
    FileManager::loadMaterials(lmsManager);
    FileManager::loadBooks(library);
    FileManager::loadExams(examManager);
    FileManager::loadExamResults(examManager);
    FileManager::loadCourses(courseManager);
    FileManager::loadNotifications(notifManager);
    FileManager::loadCalendar(calendar);
    // Restore globalMaxId so nextId() never collides with loaded records
    for (auto& s : students)  if (s.id > globalMaxId) globalMaxId = s.id;
    for (auto& t : teachers)  if (t.id > globalMaxId) globalMaxId = t.id;
    for (auto& a : admins)    if (a.id > globalMaxId) globalMaxId = a.id;
    for (auto& p : parents)   if (p.id > globalMaxId) globalMaxId = p.id;
}

void clearInput() { cin.ignore(numeric_limits<streamsize>::max(), '\n'); }

string getLine(const string& prompt) {
    string s; cout << prompt; getline(cin, s);
    return Auth::sanitize(s);
}

// ── Admin Menu ────────────────────────────────────────────────────────────────



// ── Login ─────────────────────────────────────────────────────────────────────
bool login() {
    clearInput();
    cout << "Username: "; string uname; getline(cin, uname); uname = Auth::sanitize(uname);
    cout << "Password: "; string pass;  getline(cin, pass);  pass  = Auth::sanitize(pass);

    if (!Auth::isValid(uname) || !Auth::isValid(pass)) {
        cout << "[!] Username and password cannot be empty.\n"; return false;
    }
    if (auth.isLockedOut(uname)) {
        cout << "[!] Account is locked. Please wait 15 minutes.\n"; return false;
    }

    for (auto& a : admins)
        if (a.username == uname && a.password == pass) {
            auth.recordSuccess(uname);
            Auth::showWelcomeBanner(a.name, "Admin");
            int uc = notifManager.unreadCount(a.id);
            if (uc > 0) cout << "  You have " << uc << " unread notification(s).\n";
            adminMenu(a); return true;
        }
    for (auto& t : teachers)
        if (t.username == uname && t.password == pass) {
            auth.recordSuccess(uname);
            Auth::showWelcomeBanner(t.name, "Teacher");
            int uc = notifManager.unreadCount(t.id);
            if (uc > 0) cout << "  You have " << uc << " unread notification(s).\n";
            teacherMenu(t); return true;
        }
    for (auto& s : students)
        if (s.username == uname && s.password == pass) {
            auth.recordSuccess(uname);
            Auth::showWelcomeBanner(s.name, "Student");
            cout << "  Notifications:\n"; notifManager.showAndMarkRead(s.id);
            studentMenu(s); return true;
        }
    for (auto& p : parents)
        if (p.username == uname && p.password == pass) {
            auth.recordSuccess(uname);
            Auth::showWelcomeBanner(p.name, "Parent");
            cout << "  Notifications:\n"; notifManager.showAndMarkRead(p.id);
            parentMenu(p); return true;
        }

    auth.recordFailure(uname);
    return false;
}

// ── Seed defaults ─────────────────────────────────────────────────────────────
void seedDefaults() {
    if (admins.empty())
        admins.emplace_back(1, "admin", "admin123", "System Admin");

    // Default fee structure if none loaded
    if (feeManager.getStructures().empty()) {
        feeManager.setFeeStructure("10A", 1500.00);
        feeManager.setFeeStructure("10B", 1500.00);
        feeManager.setFeeStructure("11A", 1800.00);
        feeManager.setFeeStructure("11B", 1800.00);
        feeManager.setFeeStructure("12A", 2000.00);
        feeManager.setFeeStructure("12B", 2000.00);
    }
}

// ── Main ──────────────────────────────────────────────────────────────────────
int main() {
    loadAll();
    seedDefaults();

    int choice;
    do {
        cout << "\n╔══════════════════════════════════════════╗\n"
             << "║     SCHOOL MANAGEMENT SYSTEM             ║\n"
             << "╚═════════════════════════════════════════╝\n"
             << "1. Login\n2. Exit\nSelect: ";
        cin >> choice;
        if (choice == 1) login();
    } while (choice != 2);

    saveAll();
    cout << "Goodbye!\n";
    return 0;
}
