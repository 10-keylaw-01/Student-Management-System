#pragma once
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include "Student.h"
#include "Teacher.h"
#include "Admin.h"
#include "Fee.h"
#include "Attendance.h"
#include "Grade.h"
#include "Timetable.h"
#include "Announcement.h"
#include "Assignment.h"
#include "Book.h"
#include "Exam.h"
#include "Course.h"
#include "Notification.h"
#include "Calendar.h"

/// File manager for CSV persistence
class FileManager {
    /// Wrap a field in quotes if it contains a comma or quote; escape inner quotes
    static std::string csvField(const std::string& s) {
        if (s.find(',') == std::string::npos && s.find('"') == std::string::npos) return s;
        std::string out = "\"";
        for (char c : s) { if (c == '"') out += '"'; out += c; }
        out += '"';
        return out;
    }

    static void writeCSV(const std::string& path, const std::vector<std::string>& lines) {
        std::ofstream f(path);
        if (!f.is_open()) {
            std::cerr << "[!] Error: could not open " << path << " for writing.\n";
            return;
        }
        for (const auto& l : lines) f << l << "\n";
    }

    /// Parse a single CSV line respecting double-quoted fields
    static std::vector<std::string> parseCSVLine(const std::string& line) {
        std::vector<std::string> row;
        std::string cell;
        bool inQuotes = false;
        for (size_t i = 0; i < line.size(); ++i) {
            char c = line[i];
            if (inQuotes) {
                if (c == '"') {
                    if (i + 1 < line.size() && line[i+1] == '"') { cell += '"'; ++i; }
                    else inQuotes = false;
                } else cell += c;
            } else {
                if (c == '"') inQuotes = true;
                else if (c == ',') { row.push_back(cell); cell.clear(); }
                else cell += c;
            }
        }
        row.push_back(cell);
        return row;
    }

    static std::vector<std::vector<std::string>> readCSV(const std::string& path) {
        std::vector<std::vector<std::string>> result;
        std::ifstream f(path);
        std::string line;
        while (std::getline(f, line)) {
            if (line.empty()) continue;
            result.push_back(parseCSVLine(line));
        }
        return result;
    }

public:
    // ── Students ──────────────────────────────────────────────────────────────
    static void saveStudents(const std::vector<Student>& v) {
        std::vector<std::string> lines;
        for (const auto& s : v)
            lines.push_back(std::to_string(s.id)+","+csvField(s.username)+","+csvField(s.password)+","+
                            csvField(s.name)+","+csvField(s.rollNumber)+","+csvField(s.className)+","+std::to_string(s.parentId));
        writeCSV("data/students.csv", lines);
    }
    static std::vector<Student> loadStudents() {
        std::vector<Student> v;
        for (auto& r : readCSV("data/students.csv")) {
            if (r.size() < 7) continue;
            v.emplace_back(stoi(r[0]),r[1],r[2],r[3],r[4],r[5],stoi(r[6]));
        }
        return v;
    }

    // ── Teachers ──────────────────────────────────────────────────────────────
    static void saveTeachers(const std::vector<Teacher>& v) {
        std::vector<std::string> lines;
        for (const auto& t : v) {
            // Serialize assignedClasses as pipe-separated values in column 7
            std::string classes;
            for (size_t i = 0; i < t.assignedClasses.size(); ++i) {
                if (i > 0) classes += "|";
                classes += t.assignedClasses[i];
            }
            lines.push_back(std::to_string(t.id)+","+csvField(t.username)+","+csvField(t.password)+","+
                            csvField(t.name)+","+csvField(t.subject)+","+csvField(t.department)+","+csvField(classes));
        }
        writeCSV("data/teachers.csv", lines);
    }
    static std::vector<Teacher> loadTeachers() {
        std::vector<Teacher> v;
        for (auto& r : readCSV("data/teachers.csv")) {
            if (r.size() < 6) continue;
            v.emplace_back(stoi(r[0]),r[1],r[2],r[3],r[4],r[5]);
            // Restore assignedClasses if column 7 exists
            if (r.size() >= 7 && !r[6].empty()) {
                std::string classes = r[6];
                size_t pos = 0;
                while ((pos = classes.find('|')) != std::string::npos) {
                    std::string cls = classes.substr(0, pos);
                    if (!cls.empty()) v.back().assignedClasses.push_back(cls);
                    classes.erase(0, pos + 1);
                }
                if (!classes.empty()) v.back().assignedClasses.push_back(classes);
            }
        }
        return v;
    }

    // ── Admins ────────────────────────────────────────────────────────────────
    static void saveAdmins(const std::vector<Admin>& v) {
        std::vector<std::string> lines;
        for (const auto& a : v)
            lines.push_back(std::to_string(a.id)+","+csvField(a.username)+","+csvField(a.password)+","+csvField(a.name));
        writeCSV("data/admins.csv", lines);
    }
    static std::vector<Admin> loadAdmins() {
        std::vector<Admin> v;
        for (auto& r : readCSV("data/admins.csv")) {
            if (r.size() < 4) continue;
            v.emplace_back(stoi(r[0]),r[1],r[2],r[3]);
        }
        return v;
    }

    // ── Parents ───────────────────────────────────────────────────────────────
    static void saveParents(const std::vector<Parent>& v) {
        std::vector<std::string> lines;
        for (const auto& p : v)
            lines.push_back(std::to_string(p.id)+","+csvField(p.username)+","+csvField(p.password)+","+
                            csvField(p.name)+","+std::to_string(p.studentId));
        writeCSV("data/parents.csv", lines);
    }
    static std::vector<Parent> loadParents() {
        std::vector<Parent> v;
        for (auto& r : readCSV("data/parents.csv")) {
            if (r.size() < 5) continue;
            v.emplace_back(stoi(r[0]),r[1],r[2],r[3],stoi(r[4]));
        }
        return v;
    }

    // ── Fees ──────────────────────────────────────────────────────────────────
    static void saveFees(const std::vector<FeeRecord>& v) {
        std::vector<std::string> lines;
        for (const auto& r : v)
            lines.push_back(std::to_string(r.invoiceId)+","+std::to_string(r.studentId)+","+
                            csvField(r.studentName)+","+csvField(r.className)+","+r.dueDate+","+r.paidDate+","+
                            std::to_string(r.amount)+","+std::to_string(r.paid)+","+std::to_string(r.settled));
        writeCSV("data/fees.csv", lines);
    }
    static void loadFees(FeeManager& fm) {
        int maxId = 0;
        for (auto& r : readCSV("data/fees.csv")) {
            if (r.size() < 9) continue;
            FeeRecord rec;
            rec.invoiceId=stoi(r[0]); rec.studentId=stoi(r[1]);
            rec.studentName=r[2];     rec.className=r[3];
            rec.dueDate=r[4];         rec.paidDate=r[5];
            rec.amount=stod(r[6]);    rec.paid=stod(r[7]);
            rec.settled=stoi(r[8]);
            fm.getRecords().push_back(rec);
            if (rec.invoiceId > maxId) maxId = rec.invoiceId;
        }
        fm.setNextId(maxId + 1);
    }

    // ── Fee Structure ─────────────────────────────────────────────────────────
    static void saveFeeStructure(const std::vector<FeeStructure>& v) {
        std::vector<std::string> lines;
        for (const auto& s : v)
            lines.push_back(csvField(s.className)+","+std::to_string(s.termlyFee));
        writeCSV("data/fee_structure.csv", lines);
    }
    static void loadFeeStructure(FeeManager& fm) {
        for (auto& r : readCSV("data/fee_structure.csv")) {
            if (r.size() < 2) continue;
            fm.setFeeStructure(r[0], stod(r[1]));
        }
    }

    // ── Attendance ────────────────────────────────────────────────────────────
    static void saveAttendance(const std::vector<AttendanceEntry>& v) {
        std::vector<std::string> lines;
        for (const auto& e : v)
            lines.push_back(std::to_string(e.studentId)+","+csvField(e.studentName)+","+csvField(e.className)+","+e.date+","+std::to_string(e.present));
        writeCSV("data/attendance.csv", lines);
    }
    static void loadAttendance(AttendanceManager& am) {
        for (auto& r : readCSV("data/attendance.csv")) {
            if (r.size() < 5) continue;
            am.getEntries().push_back({stoi(r[0]),r[1],r[2],r[3],(bool)stoi(r[4])});
        }
    }

    // ── Grades ────────────────────────────────────────────────────────────────
    static void saveGrades(const std::vector<GradeEntry>& v) {
        std::vector<std::string> lines;
        for (const auto& e : v)
            lines.push_back(std::to_string(e.studentId)+","+csvField(e.studentName)+","+csvField(e.subject)+","+csvField(e.term)+","+std::to_string(e.marks)+","+std::to_string(e.total));
        writeCSV("data/grades.csv", lines);
    }
    static void loadGrades(GradeManager& gm) {
        for (auto& r : readCSV("data/grades.csv")) {
            if (r.size() < 6) continue;
            gm.getEntries().push_back({stoi(r[0]),r[1],r[2],r[3],stod(r[4]),stod(r[5])});
        }
    }

    // ── Timetable ─────────────────────────────────────────────────────────────
    static void saveTimetable(const std::vector<TimeSlot>& v) {
        std::vector<std::string> lines;
        for (const auto& s : v)
            lines.push_back(csvField(s.className)+","+csvField(s.day)+","+csvField(s.startTime)+","+csvField(s.endTime)+","+csvField(s.subject)+","+csvField(s.teacherName));
        writeCSV("data/timetable.csv", lines);
    }
    static void loadTimetable(Timetable& tt) {
        for (auto& r : readCSV("data/timetable.csv")) {
            if (r.size() < 6) continue;
            tt.getSlots().push_back({r[0],r[1],r[2],r[3],r[4],r[5]});
        }
    }

    // ── Announcements ─────────────────────────────────────────────────────────
    static void saveAnnouncements(const std::vector<Announcement>& v) {
        std::vector<std::string> lines;
        for (const auto& a : v)
            lines.push_back(std::to_string(a.id)+","+csvField(a.title)+","+csvField(a.content)+","+a.date+","+
                            csvField(a.author)+","+std::to_string((int)a.targetRole)+","+std::to_string(a.allRoles));
        writeCSV("data/announcements.csv", lines);
    }
    static void loadAnnouncements(AnnouncementManager& am) {
        int maxId = 0;
        for (auto& r : readCSV("data/announcements.csv")) {
            if (r.size() < 7) continue;
            am.getAll().push_back({stoi(r[0]),r[1],r[2],r[3],r[4],(Role)stoi(r[5]),(bool)stoi(r[6])});
            if (stoi(r[0]) > maxId) maxId = stoi(r[0]);
        }
        am.setNextId(maxId + 1);
    }

    // ── Assignments ───────────────────────────────────────────────────────────
    static void saveAssignments(const std::vector<Assignment>& v) {
        std::vector<std::string> lines;
        for (const auto& a : v)
            lines.push_back(std::to_string(a.id)+","+std::to_string(a.teacherId)+","+csvField(a.title)+","+
                           csvField(a.subject)+","+a.dueDate+","+csvField(a.fileRef)+","+csvField(a.className));
        writeCSV("data/assignments.csv", lines);
    }
    static void loadAssignments(LMSManager& lms) {
        int maxId = 0;
        for (auto& r : readCSV("data/assignments.csv")) {
            if (r.size() < 7) continue;
            lms.getAssignments().push_back({stoi(r[0]),stoi(r[1]),r[2],r[3],r[4],r[5],r[6]});
            if (stoi(r[0]) > maxId) maxId = stoi(r[0]);
        }
        // Update nextId for assignments
        int mMaxId = 0;
        for (const auto& m : lms.getMaterials()) if (m.id > mMaxId) mMaxId = m.id;
        lms.setNextIds(maxId + 1, mMaxId + 1);
    }

    // ── Materials ─────────────────────────────────────────────────────────────
    static void saveMaterials(const std::vector<Material>& v) {
        std::vector<std::string> lines;
        for (const auto& m : v)
            lines.push_back(std::to_string(m.id)+","+std::to_string(m.teacherId)+","+csvField(m.title)+","+
                           csvField(m.subject)+","+csvField(m.contentPath)+","+csvField(m.className));
        writeCSV("data/materials.csv", lines);
    }
    static void loadMaterials(LMSManager& lms) {
        int maxId = 0;
        for (auto& r : readCSV("data/materials.csv")) {
            if (r.size() < 6) continue;
            lms.getMaterials().push_back({stoi(r[0]),stoi(r[1]),r[2],r[3],r[4],r[5]});
            if (stoi(r[0]) > maxId) maxId = stoi(r[0]);
        }
        // Update nextId for materials
        int aMaxId = 0;
        for (const auto& a : lms.getAssignments()) if (a.id > aMaxId) aMaxId = a.id;
        lms.setNextIds(aMaxId + 1, maxId + 1);
    }

    // ── Books ─────────────────────────────────────────────────────────────────
    static void saveBooks(const std::vector<Book>& v) {
        std::vector<std::string> lines;
        for (const auto& b : v)
            lines.push_back(std::to_string(b.id)+","+csvField(b.title)+","+csvField(b.author)+","+
                           std::to_string(b.available)+","+std::to_string(b.issuedTo)+","+csvField(b.issueDate)+","+
                           csvField(b.dueDate)+","+csvField(b.returnDate));
        writeCSV("data/books.csv", lines);
    }
    static void loadBooks(Library& lib) {
        int maxId = 0;
        for (auto& r : readCSV("data/books.csv")) {
            if (r.size() < 6) continue;
            Book b{stoi(r[0]),r[1],r[2],(bool)stoi(r[3]),stoi(r[4]),r[5],"",""};
            // Load dueDate and returnDate if they exist
            if (r.size() >= 7) b.dueDate = r[6];
            if (r.size() >= 8) b.returnDate = r[7];
            lib.getBooks().push_back(b);
            if (b.id > maxId) maxId = b.id;
        }
        lib.setNextId(maxId + 1);
    }

    // ── Exams ─────────────────────────────────────────────────────────────────
    static void saveExams(const std::vector<Exam>& v) {
        std::vector<std::string> lines;
        for (const auto& e : v)
            lines.push_back(std::to_string(e.id)+","+csvField(e.title)+","+csvField(e.type)+","+csvField(e.className)+","+csvField(e.subject)+","+e.date+","+std::to_string(e.totalMarks));
        writeCSV("data/exams.csv", lines);
    }
    static void loadExams(ExamManager& em) {
        int maxId = 0;
        for (auto& r : readCSV("data/exams.csv")) {
            if (r.size() < 7) continue;
            Exam e{stoi(r[0]),r[1],r[2],r[3],r[4],r[5],stod(r[6])};
            em.getExams().push_back(e);
            if (e.id > maxId) maxId = e.id;
        }
        em.setNextIds(maxId + 1, 1);
    }

    static void saveExamResults(const std::vector<ExamResult>& v) {
        std::vector<std::string> lines;
        for (const auto& r : v)
            lines.push_back(std::to_string(r.id)+","+std::to_string(r.examId)+","+std::to_string(r.studentId)+","+csvField(r.studentName)+","+csvField(r.examTitle)+","+csvField(r.subject)+","+csvField(r.className)+","+
                            std::to_string(r.marksObtained)+","+std::to_string(r.totalMarks));
        writeCSV("data/exam_results.csv", lines);
    }
    static void loadExamResults(ExamManager& em) {
        int maxId = 0;
        for (auto& r : readCSV("data/exam_results.csv")) {
            if (r.size() < 9) continue;
            ExamResult res{stoi(r[0]),stoi(r[1]),stoi(r[2]),r[3],r[4],r[5],r[6],stod(r[7]),stod(r[8])};
            em.getResults().push_back(res);
            if (res.id > maxId) maxId = res.id;
        }
        // Only update result ID counter, preserve exam ID counter
        if (maxId > 0) {
            int eMax = em.getExams().empty() ? 0 : em.getExams().back().id;
            em.setNextIds(eMax + 1, maxId + 1);
        }
    }

    // ── Courses ───────────────────────────────────────────────────────────────
    static void saveCourses(const std::vector<Course>& v) {
        std::vector<std::string> lines;
        for (const auto& c : v)
            lines.push_back(std::to_string(c.id)+","+csvField(c.code)+","+csvField(c.name)+","+csvField(c.subject)+","+csvField(c.className)+","+csvField(c.teacherName)+","+std::to_string(c.credits));
        writeCSV("data/courses.csv", lines);
    }
    static void loadCourses(CourseManager& cm) {
        int maxId = 0;
        for (auto& r : readCSV("data/courses.csv")) {
            if (r.size() < 7) continue;
            Course c{stoi(r[0]),r[1],r[2],r[3],r[4],r[5],stoi(r[6])};
            cm.getCourses().push_back(c);
            if (c.id > maxId) maxId = c.id;
        }
        cm.setNextId(maxId + 1);
    }

    // ── Notifications ─────────────────────────────────────────────────────────
    static void saveNotifications(const std::vector<Notification>& v) {
        std::vector<std::string> lines;
        for (const auto& n : v)
            lines.push_back(std::to_string(n.id)+","+std::to_string(n.userId)+","+
                            csvField(n.message)+","+n.date+","+csvField(n.category)+","+std::to_string(n.read));
        writeCSV("data/notifications.csv", lines);
    }
    static void loadNotifications(NotificationManager& nm) {
        int maxId = 0;
        for (auto& r : readCSV("data/notifications.csv")) {
            if (r.size() < 6) continue;
            Notification n{stoi(r[0]),stoi(r[1]),r[2],r[3],r[4],(bool)stoi(r[5])};
            nm.getAll().push_back(n);
            if (n.id > maxId) maxId = n.id;
        }
        nm.setNextId(maxId + 1);
    }

    // ── Calendar ──────────────────────────────────────────────────────────────
    static void saveCalendar(const std::vector<CalendarEvent>& v) {
        std::vector<std::string> lines;
        for (const auto& e : v)
            lines.push_back(std::to_string(e.id)+","+csvField(e.title)+","+e.date+","+e.endDate+","+csvField(e.type)+","+csvField(e.description));
        writeCSV("data/calendar.csv", lines);
    }
    static void loadCalendar(AcademicCalendar& cal) {
        int maxId = 0;
        for (auto& r : readCSV("data/calendar.csv")) {
            if (r.size() < 6) continue;
            CalendarEvent e{stoi(r[0]),r[1],r[2],r[3],r[4],r[5]};
            cal.getEvents().push_back(e);
            if (e.id > maxId) maxId = e.id;
        }
        cal.setNextId(maxId + 1);
    }
};
