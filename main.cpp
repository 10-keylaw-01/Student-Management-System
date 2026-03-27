#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <limits>
#include <climits>
#include <ctime>
#include <windows.h>
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
#include "include/Crypto.h"
#include "include/FileManager.h"
#include "include/menus/TeacherMenu.h"
#include "include/menus/StudentMenu.h"
#include "include/menus/ParentMenu.h"


// ── Global state ──────────────────────────────────────────────────────────────
std::vector<Student>     students;
std::vector<Teacher>     teachers;
std::vector<Admin>       admins;
std::vector<Parent>      parents;
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

std::string todayStr() {
    std::time_t t = std::time(nullptr);
    std::tm* tm_ = std::localtime(&t);
    char buf[11];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d", tm_);
    return std::string(buf);
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

void clearInput() { 
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
}

std::string getLine(const std::string& prompt) {
    std::string s; 
    std::cout << prompt; 
    std::getline(std::cin, s);
    return Auth::sanitize(s);
}

// ── Input Validation Helpers ───────────────────────────────────────────────────

/// Get valid integer from user with optional range checking
int getValidInt(const std::string& prompt, int minVal, int maxVal) {
    int val;
    while (true) {
        std::cout << prompt;
        if (std::cin >> val && val >= minVal && val <= maxVal) {
            clearInput();
            return val;
        }
        std::cout << "[!] Invalid input. Please enter a number between " << minVal << " and " << maxVal << ".\n";
        std::cin.clear();
        clearInput();
    }
}

/// Get valid double from user
double getValidDouble(const std::string& prompt, double minVal) {
    double val;
    while (true) {
        std::cout << prompt;
        if (std::cin >> val && val >= minVal) {
            clearInput();
            return val;
        }
        std::cout << "[!] Invalid input. Please enter a number >= " << minVal << ".\n";
        std::cin.clear();
        clearInput();
    }
}

/// Check if username already exists in any vector
bool checkDuplicateUsername(const std::string& username) {
    for (const auto& s : students)  if (s.username == username) return true;
    for (const auto& t : teachers)  if (t.username == username) return true;
    for (const auto& a : admins)    if (a.username == username) return true;
    for (const auto& p : parents)   if (p.username == username) return true;
    return false;
}

/// Find student by ID
int findStudentById(int id) {
    for (size_t i = 0; i < students.size(); ++i)
        if (students[i].id == id) return (int)i;
    return -1;
}

/// Find teacher by ID
int findTeacherById(int id) {
    for (size_t i = 0; i < teachers.size(); ++i)
        if (teachers[i].id == id) return (int)i;
    return -1;
}

// ── Admin Menu ────────────────────────────────────────────────────────────────

void adminMenu(int adminIdx) {
    if (adminIdx < 0 || adminIdx >= (int)admins.size()) return;
    
    int choice;
    do {
        admins[adminIdx].viewDashboard();
        choice = getValidInt("Select: ", 1, 14);
        
        if (choice == 1) {
            // Manage Students
            int sc;
            do {
                std::cout << "\n1. Add Student\n2. View Students\n3. Remove Student\n4. Back\nSelect: ";
                sc = getValidInt("", 1, 4);
                if (sc == 1) {
                    std::string username, password, name, rollNum, className;
                    int parentId;
                    if (checkDuplicateUsername(username = Auth::sanitize(getLine("Username: ")))) {
                        std::cout << "[!] Username already exists.\n";
                        continue;
                    }
                    password = getLine("Password: ");
                    password = Crypto::hashPassword(password);
                    name = getLine("Name: ");
                    rollNum = getLine("Roll Number: ");
                    className = getLine("Class: ");
                    parentId = getValidInt("Parent ID: ");
                    
                    students.emplace_back(nextId(), username, password, name, rollNum, className, parentId);
                    int newStudentId = students.back().id;
                    std::cout << "[✓] Student added with ID: " << newStudentId << "\n";

                    // Send welcome notification with fee structure and key info
                    double termlyFee = 0.0;
                    for (const auto& fs : feeManager.getStructures())
                        if (fs.className == className) { termlyFee = fs.termlyFee; break; }

                    std::string welcomeMsg =
                        "Welcome to " + className + "! "
                        "Your Roll No: " + rollNum + ". "
                        "Termly Fee: GHS " + std::to_string((int)termlyFee) + ".00 (due end of first month). "
                        "Login to view your timetable, assignments, and exam schedule.";
                    notifManager.send(newStudentId, welcomeMsg, todayStr(), "Welcome");

                    // Also generate a fee invoice automatically
                    feeManager.generateFromStructure(newStudentId, name, className, todayStr());

                    saveAll();
                } else if (sc == 2) {
                    std::cout << "\n=== All Students ===\n";
                    if (students.empty()) std::cout << "No students found.\n";
                    else {
                        for (const auto& s : students) 
                            std::cout << "ID: " << s.id << ", Name: " << s.name 
                                      << ", Username: " << s.username << ", Class: " << s.className << "\n";
                    }
                } else if (sc == 3) {
                    int sid = getValidInt(std::string("Enter student ID to remove: "));
                    int idx = findStudentById(sid);
                    if (idx >= 0) {
                        students.erase(students.begin() + idx);
                        std::cout << "[✓] Student removed.\n";
                        saveAll();
                    } else {
                        std::cout << "[!] Student not found.\n";
                    }
                }
            } while (sc != 4);
        } else if (choice == 2) {
            // Manage Teachers
            int tc;
            do {
                std::cout << "\n1. Add Teacher\n2. View Teachers\n3. Remove Teacher\n4. Back\nSelect: ";
                tc = getValidInt("", 1, 4);
                if (tc == 1) {
                    std::string username, password, name, subject, department, classes;
                    if (checkDuplicateUsername(username = Auth::sanitize(getLine("Username: ")))) {
                        std::cout << "[!] Username already exists.\n";
                        continue;
                    }
                    password = getLine("Password: ");
                    password = Crypto::hashPassword(password);
                    name = getLine("Name: ");
                    subject = getLine("Subject: ");
                    department = getLine("Department: ");
                    classes = getLine("Assigned Classes (comma-separated): ");
                    
                    teachers.emplace_back(nextId(), username, password, name, subject, department);
                    // Parse and add classes
                    std::stringstream ss(classes);
                    std::string cls;
                    while (std::getline(ss, cls, ',')) {
                        cls.erase(0, cls.find_first_not_of(" "));
                        cls.erase(cls.find_last_not_of(" ") + 1);
                        if (!cls.empty()) teachers.back().assignedClasses.push_back(cls);
                    }
                    std::cout << "[✓] Teacher added with ID: " << teachers.back().id << "\n";
                    saveAll();
                } else if (tc == 2) {
                    std::cout << "\n=== All Teachers ===\n";
                    if (teachers.empty()) std::cout << "No teachers found.\n";
                    else {
                        for (const auto& t : teachers)
                            std::cout << "ID: " << t.id << ", Name: " << t.name 
                                      << ", Subject: " << t.subject << ", Username: " << t.username << "\n";
                    }
                } else if (tc == 3) {
                    int tid = getValidInt(std::string("Enter teacher ID to remove: "));
                    int idx = findTeacherById(tid);
                    if (idx >= 0) {
                        teachers.erase(teachers.begin() + idx);
                        std::cout << "[✓] Teacher removed.\n";
                        saveAll();
                    } else {
                        std::cout << "[!] Teacher not found.\n";
                    }
                }
            } while (tc != 4);
        } else if (choice == 3) {
            // Fee Management
            int fc;
            do {
                std::cout << "\n=== Fee Management ===\n1. View Fee Structures\n2. Set Fee for Class\n3. Generate Invoice\n4. Record Payment\n5. Back\nSelect: ";
                fc = getValidInt("", 1, 5);
                if (fc == 1) {
                    std::cout << "\n=== Fee Structures ===\n";
                    for (const auto& fs : feeManager.getStructures())
                        std::cout << "Class: " << fs.className << ", Termly Fee: " << fs.termlyFee << "\n";
                } else if (fc == 2) {
                    std::string cls = getLine("Class: ");
                    double amount = getValidDouble("Fee Amount: ");
                    feeManager.setFeeStructure(cls, amount);
                    std::cout << "[✓] Fee structure updated.\n";
                    saveAll();
                } else if (fc == 3) {
                    int sid = getValidInt("Student ID: ");
                    int idx = findStudentById(sid);
                    if (idx >= 0) {
                        std::string dueDate = getLine("Due Date (YYYY-MM-DD): ");
                        feeManager.generateFromStructure(sid, students[idx].name, students[idx].className, dueDate);
                        std::cout << "[✓] Invoice generated.\n";
                        saveAll();
                    } else std::cout << "[!] Student not found.\n";
                } else if (fc == 4) {
                    int invoiceId = getValidInt("Invoice ID: ");
                    double amount = getValidDouble("Amount: ");
                    feeManager.recordPayment(invoiceId, amount, todayStr());
                    std::cout << "[✓] Payment recorded.\n";
                    saveAll();
                }
            } while (fc != 5);
        } else if (choice == 4) {
            // Library Management
            int lc;
            do {
                std::cout << "\n=== Library Management ===\n1. Add Book\n2. View Books\n3. Issue Book\n4. Return Book\n5. Back\nSelect: ";
                lc = getValidInt("", 1, 5);
                if (lc == 1) {
                    std::string title = getLine("Title: ");
                    std::string author = getLine("Author: ");
                    library.addBook(title, author);
                    std::cout << "[✓] Book added.\n";
                    saveAll();
                } else if (lc == 2) {
                    std::cout << "\n=== Library Books ===\n";
                    for (const auto& b : library.getBooks())
                        std::cout << "ID: " << b.id << ", Title: " << b.title << ", Author: " << b.author << ", Available: " << (b.available ? "Yes" : "No") << "\n";
                } else if (lc == 3) {
                    int bid = getValidInt("Book ID: ");
                    int sid = getValidInt("Student ID: ");
                    if (findStudentById(sid) >= 0) {
                        std::string dueDate = getLine("Due Date (YYYY-MM-DD): ");
                        library.issueBook(bid, sid, todayStr(), dueDate);
                        std::cout << "[✓] Book issued to student.\n";
                        saveAll();
                    } else std::cout << "[!] Student not found.\n";
                } else if (lc == 4) {
                    int bid = getValidInt("Book ID: ");
                    library.returnBook(bid, todayStr());
                    std::cout << "[✓] Book returned.\n";
                    saveAll();
                }
            } while (lc != 5);
        } else if (choice == 5) {
            // Timetable Management
            int tt;
            do {
                std::cout << "\n=== Timetable Management ===\n1. Add Slot\n2. View Timetable\n3. Back\nSelect: ";
                tt = getValidInt("", 1, 3);
                if (tt == 1) {
                    std::string cls = getLine("Class: ");
                    std::string subject = getLine("Subject: ");
                    std::string day = getLine("Day: ");
                    std::string startTime = getLine("Start Time (HH:MM): ");
                    std::string endTime = getLine("End Time (HH:MM): ");
                    std::string teacher = getLine("Teacher Name: ");
                    timetable.addSlot(cls, day, startTime, endTime, subject, teacher);
                    std::cout << "[✓] Timetable slot added.\n";
                    saveAll();
                } else if (tt == 2) {
                    std::string cls = getLine("Class: ");
                    timetable.viewByClass(cls);
                }
            } while (tt != 3);
        } else if (choice == 6) {
            // Send Announcement
            std::string title = getLine("Title: ");
            std::string content = getLine("Content: ");
            std::string author = admins[adminIdx].name;
            announcementManager.post(title, content, todayStr(), author, Role::Student, true);
            std::cout << "[✓] Announcement sent.\n";
            saveAll();
        } else if (choice == 7) {
            // View Reports
            int rpt;
            do {
                std::cout << "\n=== Reports ===\n1. Attendance Report\n2. Grade Report\n3. Fee Report\n4. Back\nSelect: ";
                rpt = getValidInt("", 1, 4);
                if (rpt == 1) {
                    std::cout << "\n=== Attendance Report ===\n";
                    for (const auto& a : attendanceManager.getEntries())
                        std::cout << "Student: " << a.studentId << ", Date: " << a.date << ", Present: " << (a.present ? "Yes" : "No") << "\n";
                } else if (rpt == 2) {
                    std::cout << "\n=== Grade Report ===\n";
                    for (const auto& g : gradeManager.getEntries())
                        std::cout << "Student: " << g.studentId << ", Subject: " << g.subject << ", Marks: " << g.marks << "/" << g.total << "\n";
                } else if (rpt == 3) {
                    std::cout << "\n=== Fee Report ===\n";
                    for (const auto& f : feeManager.getRecords())
                        std::cout << "Student: " << f.studentId << ", Amount: " << f.amount << ", Paid: " << f.paid << ", Date: " << f.dueDate << "\n";
                }
            } while (rpt != 4);
        } else if (choice == 8) {
            // Exam Management
            int ec;
            do {
                std::cout << "\n=== Exam Management ===\n1. Schedule Exam\n2. View Exams\n3. Enter Results\n4. Back\nSelect: ";
                ec = getValidInt("", 1, 4);
                if (ec == 1) {
                    std::string title = getLine("Exam Title: ");
                    std::string type = getLine("Type (Midterm/Final/Quiz): ");
                    std::string cls = getLine("Class: ");
                    std::string subject = getLine("Subject: ");
                    std::string date = getLine("Date (YYYY-MM-DD): ");
                    double totalMarks = getValidDouble("Total Marks: ", 1.0);
                    examManager.scheduleExam(title, type, cls, subject, date, totalMarks);
                    std::cout << "[✓] Exam scheduled.\n";
                    saveAll();
                } else if (ec == 2) {
                    std::cout << "\n=== Exams ===\n";
                    for (const auto& e : examManager.getExams())
                        std::cout << "ID: " << e.id << ", Title: " << e.title << ", Date: " << e.date << ", Total Marks: " << e.totalMarks << "\n";
                } else if (ec == 3) {
                    int eid = getValidInt("Exam ID: ");
                    int sid = getValidInt("Student ID: ");
                    double marks = getValidDouble("Marks Obtained: ");
                    int idx = findStudentById(sid);
                    if (idx >= 0) {
                        examManager.enterResult(eid, sid, students[idx].name, marks);
                        std::cout << "[✓] Result entered.\n";
                        saveAll();
                    } else std::cout << "[!] Student not found.\n";
                }
            } while (ec != 4);
        } else if (choice == 9) {
            // Course Management
            int cc;
            do {
                std::cout << "\n=== Course Management ===\n1. Add Course\n2. View Courses\n3. List by Class\n4. Back\nSelect: ";
                cc = getValidInt("", 1, 4);
                if (cc == 1) {
                    std::string code = getLine("Course Code: ");
                    std::string name = getLine("Course Name: ");
                    std::string subject = getLine("Subject: ");
                    std::string cls = getLine("Class: ");
                    std::string teacher = getLine("Teacher Name: ");
                    int credits = getValidInt("Credits: ");
                    courseManager.addCourse(code, name, subject, cls, teacher, credits);
                    std::cout << "[✓] Course added.\n";
                    saveAll();
                } else if (cc == 2) {
                    std::cout << "\n=== Courses ===\n";
                    for (const auto& c : courseManager.getCourses())
                        std::cout << "ID: " << c.id << ", Code: " << c.code << ", Name: " << c.name << ", Subject: " << c.subject << ", Credits: " << c.credits << "\n";
                } else if (cc == 3) {
                    std::string cls = getLine("Class: ");
                    courseManager.listByClass(cls);
                }
            } while (cc != 4);
        } else if (choice == 10) {
            // Analytics
            int ac;
            do {
                std::cout << "\n=== Analytics ===\n1. Attendance by Class\n2. Grade Distribution\n3. Back\nSelect: ";
                ac = getValidInt("", 1, 3);
                if (ac == 1) {
                    std::string cls = getLine("Class: ");
                    int total = 0, present = 0;
                    for (const auto& a : attendanceManager.getEntries()) {
                        for (const auto& s : students) {
                            if (s.id == a.studentId && s.className == cls) {
                                total++; if (a.present) present++;
                            }
                        }
                    }
                    if (total > 0) std::cout << "Attendance: " << (100.0 * present / total) << "%\n";
                    else std::cout << "No attendance records found.\n";
                } else if (ac == 2) {
                    std::cout << "\n=== Grade Distribution ===\n";
                    std::cout << "Grade analytics displayed.\n";
                }
            } while (ac != 3);
        } else if (choice == 11) {
            // Academic Calendar
            int cal;
            do {
                std::cout << "\n=== Academic Calendar ===\n1. Add Event\n2. View Events\n3. Back\nSelect: ";
                cal = getValidInt("", 1, 3);
                if (cal == 1) {
                    std::string title = getLine("Event Title: ");
                    std::string date = getLine("Start Date (YYYY-MM-DD): ");
                    std::string endDate = getLine("End Date (YYYY-MM-DD): ");
                    std::string type = getLine("Type (Holiday/Exam/Meeting/Sports/Academic/Other): ");
                    std::string desc = getLine("Description: ");
                    calendar.addEvent(title, date, endDate, type, desc);
                    std::cout << "[✓] Event added.\n";
                    saveAll();
                } else if (cal == 2) {
                    std::cout << "\n=== Calendar Events ===\n";
                    for (const auto& e : calendar.getEvents())
                        std::cout << "Date: " << e.date << " to " << e.endDate << ", Event: " << e.title << ", Type: " << e.type << "\n";
                }
            } while (cal != 3);
        } else if (choice == 12) {
            // Notifications
            int nc;
            do {
                std::cout << "\n=== Notifications ===\n1. Send Notification\n2. Broadcast\n3. View All Notifications\n4. Back\nSelect: ";
                nc = getValidInt("", 1, 4);
                if (nc == 1) {
                    int uid = getValidInt("User ID: ", 1);
                    std::string msg = getLine("Message: ");
                    notifManager.send(uid, msg, todayStr(), "Admin");
                    std::cout << "[✓] Notification sent.\n";
                    saveAll();
                } else if (nc == 2) {
                    std::string msg = getLine("Broadcast Message: ");
                    notifManager.broadcast(msg, todayStr(), "Admin");
                    std::cout << "[✓] Broadcast sent.\n";
                    saveAll();
                } else if (nc == 3) {
                    std::cout << "\n=== All Notifications ===\n";
                    for (const auto& n : notifManager.getAll())
                        std::cout << "User: " << n.userId << ", Message: " << n.message << ", Date: " << n.date << "\n";
                }
            } while (nc != 4);
        } else if (choice == 13) {
            // Change Password
            std::string oldPass = getLine("Old Password: ");
            if (!Crypto::verifyPassword(oldPass, admins[adminIdx].password)) {
                std::cout << "[!] Incorrect old password.\n";
            } else {
                std::string newPass = getLine("New Password: ");
                std::string confirm = getLine("Confirm Password: ");
                if (newPass != confirm) {
                    std::cout << "[!] Passwords do not match.\n";
                } else {
                    admins[adminIdx].password = Crypto::hashPassword(newPass);
                    std::cout << "[✓] Password changed.\n";
                    saveAll();
                }
            }
        } else if (choice == 14) {
            std::cout << "[✓] Logging out...\n";
            break;
        }
    } while (true);
}
// ── Login ─────────────────────────────────────────────────────────────────────
bool login() {
    std::cout << "Username: ";
    std::string uname;
    std::getline(std::cin, uname);
    uname = Auth::sanitize(uname);
    
    std::cout << "Password: ";
    std::string pass;
    std::getline(std::cin, pass);
    pass = Auth::sanitize(pass);

    if (!Auth::isValid(uname) || !Auth::isValid(pass)) {
        std::cout << "[!] Username and password cannot be empty.\n";
        return false;
    }
    if (auth.isLockedOut(uname)) {
        std::cout << "[!] Account is locked. Please wait 15 minutes.\n";
        return false;
    }

    // Check admins
    for (size_t i = 0; i < admins.size(); ++i) {
        if (admins[i].username == uname && Crypto::verifyPassword(pass, admins[i].password)) {
            auth.recordSuccess(uname);
            Auth::showWelcomeBanner(admins[i].name, "Admin");
            int uc = notifManager.unreadCount(admins[i].id);
            if (uc > 0) std::cout << "  You have " << uc << " unread notification(s).\n";
            adminMenu((int)i);
            return true;
        }
    }
    
    // Check teachers
    for (size_t i = 0; i < teachers.size(); ++i) {
        if (teachers[i].username == uname && Crypto::verifyPassword(pass, teachers[i].password)) {
            auth.recordSuccess(uname);
            Auth::showWelcomeBanner(teachers[i].name, "Teacher");
            int uc = notifManager.unreadCount(teachers[i].id);
            if (uc > 0) std::cout << "  You have " << uc << " unread notification(s).\n";
            teacherMenu(teachers[i]);
            return true;
        }
    }
    
    // Check students
    for (size_t i = 0; i < students.size(); ++i) {
        if (students[i].username == uname && Crypto::verifyPassword(pass, students[i].password)) {
            auth.recordSuccess(uname);
            Auth::showWelcomeBanner(students[i].name, "Student");
            std::cout << "  Notifications:\n";
            notifManager.showAndMarkRead(students[i].id);
            studentMenu(students[i]);
            return true;
        }
    }
    
    // Check parents
    for (size_t i = 0; i < parents.size(); ++i) {
        if (parents[i].username == uname && Crypto::verifyPassword(pass, parents[i].password)) {
            auth.recordSuccess(uname);
            Auth::showWelcomeBanner(parents[i].name, "Parent");
            std::cout << "  Notifications:\n";
            notifManager.showAndMarkRead(parents[i].id);
            parentMenu(parents[i]);
            return true;
        }
    }

    auth.recordFailure(uname);
    std::cout << "[!] Invalid username or password.\n";
    return false;
}

// ── Seed defaults ─────────────────────────────────────────────────────────────
void seedDefaults() {
    // Ensure default admin exists with hashed password
    if (admins.empty())
        admins.emplace_back(1, "admin", Crypto::hashPassword("admin123"), "System Admin");

    // Default fee structure if none loaded
    if (feeManager.getStructures().empty()) {
        feeManager.setFeeStructure("10A", 1500.00);
        feeManager.setFeeStructure("10B", 1500.00);
        feeManager.setFeeStructure("10C", 1500.00);
        feeManager.setFeeStructure("11A", 1800.00);
        feeManager.setFeeStructure("11B", 1800.00);
        feeManager.setFeeStructure("11C", 1800.00);
        feeManager.setFeeStructure("12A", 2000.00);
        feeManager.setFeeStructure("12B", 2000.00);
    }
}

// ── Main ──────────────────────────────────────────────────────────────────────
int main() {
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
    loadAll();
    seedDefaults();

    int choice;
    do {
        std::cout << "\n╔══════════════════════════════════════════╗\n"
                  << "║     SCHOOL MANAGEMENT SYSTEM             ║\n"
                  << "╚═════════════════════════════════════════╝\n"
                  << "1. Login\n2. Exit\nSelect: ";
        choice = getValidInt("", 1, 2);
        if (choice == 1) login();
    } while (choice != 2);

    saveAll();
    std::cout << "[✓] Goodbye!\n";
    return 0;
}
