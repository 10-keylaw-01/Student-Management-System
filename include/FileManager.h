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
using namespace std;

class FileManager {
    // Wrap a field in quotes if it contains a comma or quote; escape inner quotes
    static string csvField(const string& s) {
        if (s.find(',') == string::npos && s.find('"') == string::npos) return s;
        string out = "\"";
        for (char c : s) { if (c == '"') out += '"'; out += c; }
        out += '"';
        return out;
    }

    static void writeCSV(const string& path, const vector<string>& lines) {
        ofstream f(path);
        for (auto& l : lines) f << l << "\n";
    }

    // Parse a single CSV line respecting double-quoted fields
    static vector<string> parseCSVLine(const string& line) {
        vector<string> row;
        string cell;
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

    static vector<vector<string>> readCSV(const string& path) {
        vector<vector<string>> result;
        ifstream f(path);
        string line;
        while (getline(f, line)) {
            if (line.empty()) continue;
            result.push_back(parseCSVLine(line));
        }
        return result;
    }

public:
    // ── Students ──────────────────────────────────────────────────────────────
    static void saveStudents(const vector<Student>& v) {
        vector<string> lines;
        for (auto& s : v)
            lines.push_back(to_string(s.id)+","+csvField(s.username)+","+csvField(s.password)+","+
                            csvField(s.name)+","+csvField(s.rollNumber)+","+csvField(s.className)+","+to_string(s.parentId));
        writeCSV("data/students.csv", lines);
    }
    static vector<Student> loadStudents() {
        vector<Student> v;
        for (auto& r : readCSV("data/students.csv")) {
            if (r.size() < 7) continue;
            v.emplace_back(stoi(r[0]),r[1],r[2],r[3],r[4],r[5],stoi(r[6]));
        }
        return v;
    }

    // ── Teachers ──────────────────────────────────────────────────────────────
    static void saveTeachers(const vector<Teacher>& v) {
        vector<string> lines;
        for (auto& t : v)
            lines.push_back(to_string(t.id)+","+csvField(t.username)+","+csvField(t.password)+","+
                            csvField(t.name)+","+csvField(t.subject)+","+csvField(t.department));
        writeCSV("data/teachers.csv", lines);
    }
    static vector<Teacher> loadTeachers() {
        vector<Teacher> v;
        for (auto& r : readCSV("data/teachers.csv")) {
            if (r.size() < 6) continue;
            v.emplace_back(stoi(r[0]),r[1],r[2],r[3],r[4],r[5]);
        }
        return v;
    }

    // ── Admins ────────────────────────────────────────────────────────────────
    static void saveAdmins(const vector<Admin>& v) {
        vector<string> lines;
        for (auto& a : v)
            lines.push_back(to_string(a.id)+","+csvField(a.username)+","+csvField(a.password)+","+csvField(a.name));
        writeCSV("data/admins.csv", lines);
    }
    static vector<Admin> loadAdmins() {
        vector<Admin> v;
        for (auto& r : readCSV("data/admins.csv")) {
            if (r.size() < 4) continue;
            v.emplace_back(stoi(r[0]),r[1],r[2],r[3]);
        }
        return v;
    }

    // ── Parents ───────────────────────────────────────────────────────────────
    static void saveParents(const vector<Parent>& v) {
        vector<string> lines;
        for (auto& p : v)
            lines.push_back(to_string(p.id)+","+csvField(p.username)+","+csvField(p.password)+","+
                            csvField(p.name)+","+to_string(p.studentId));
        writeCSV("data/parents.csv", lines);
    }
    static vector<Parent> loadParents() {
        vector<Parent> v;
        for (auto& r : readCSV("data/parents.csv")) {
            if (r.size() < 5) continue;
            v.emplace_back(stoi(r[0]),r[1],r[2],r[3],stoi(r[4]));
        }
        return v;
    }

    // ── Fees ──────────────────────────────────────────────────────────────────
    static void saveFees(const vector<FeeRecord>& v) {
        vector<string> lines;
        for (auto& r : v)
            lines.push_back(to_string(r.invoiceId)+","+to_string(r.studentId)+","+
                            csvField(r.studentName)+","+csvField(r.className)+","+r.dueDate+","+r.paidDate+","+
                            to_string(r.amount)+","+to_string(r.paid)+","+to_string(r.settled));
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
    static void saveFeeStructure(const vector<FeeStructure>& v) {
        vector<string> lines;
        for (auto& s : v)
            lines.push_back(csvField(s.className)+","+to_string(s.termlyFee));
        writeCSV("data/fee_structure.csv", lines);
    }
    static void loadFeeStructure(FeeManager& fm) {
        for (auto& r : readCSV("data/fee_structure.csv")) {
            if (r.size() < 2) continue;
            fm.setFeeStructure(r[0], stod(r[1]));
        }
    }

    // ── Attendance ────────────────────────────────────────────────────────────
    static void saveAttendance(const vector<AttendanceEntry>& v) {
        vector<string> lines;
        for (auto& e : v)
            lines.push_back(to_string(e.studentId)+","+csvField(e.studentName)+","+csvField(e.className)+","+e.date+","+to_string(e.present));
        writeCSV("data/attendance.csv", lines);
    }
    static void loadAttendance(AttendanceManager& am) {
        for (auto& r : readCSV("data/attendance.csv")) {
            if (r.size() < 5) continue;
            am.getEntries().push_back({stoi(r[0]),r[1],r[2],r[3],(bool)stoi(r[4])});
        }
    }

    // ── Grades ────────────────────────────────────────────────────────────────
    static void saveGrades(const vector<GradeEntry>& v) {
        vector<string> lines;
        for (auto& e : v)
            lines.push_back(to_string(e.studentId)+","+csvField(e.studentName)+","+csvField(e.subject)+","+csvField(e.term)+","+to_string(e.marks)+","+to_string(e.total));
        writeCSV("data/grades.csv", lines);
    }
    static void loadGrades(GradeManager& gm) {
        for (auto& r : readCSV("data/grades.csv")) {
            if (r.size() < 6) continue;
            gm.getEntries().push_back({stoi(r[0]),r[1],r[2],r[3],stod(r[4]),stod(r[5])});
        }
    }

    // ── Timetable ─────────────────────────────────────────────────────────────
    static void saveTimetable(const vector<TimeSlot>& v) {
        vector<string> lines;
        for (auto& s : v)
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
    static void saveAnnouncements(const vector<Announcement>& v) {
        vector<string> lines;
        for (auto& a : v)
            lines.push_back(to_string(a.id)+","+csvField(a.title)+","+csvField(a.content)+","+a.date+","+
                            csvField(a.author)+","+to_string((int)a.targetRole)+","+to_string(a.allRoles));
        writeCSV("data/announcements.csv", lines);
    }
    static void loadAnnouncements(AnnouncementManager& am) {
        for (auto& r : readCSV("data/announcements.csv")) {
            if (r.size() < 7) continue;
            am.getAll().push_back({stoi(r[0]),r[1],r[2],r[3],r[4],
                                   (Role)stoi(r[5]),(bool)stoi(r[6])});
        }
    }

    // ── Assignments ───────────────────────────────────────────────────────────
    static void saveAssignments(const vector<Assignment>& v) {
        vector<string> lines;
        for (auto& a : v)
            lines.push_back(to_string(a.id)+","+to_string(a.teacherId)+","+csvField(a.title)+","+csvField(a.subject)+","+a.dueDate+","+csvField(a.fileRef)+","+csvField(a.className));
        writeCSV("data/assignments.csv", lines);
    }
    static void loadAssignments(LMSManager& lms) {
        for (auto& r : readCSV("data/assignments.csv")) {
            if (r.size() < 7) continue;
            lms.getAssignments().push_back({stoi(r[0]),stoi(r[1]),r[2],r[3],r[4],r[5],r[6]});
        }
    }

    // ── Materials ─────────────────────────────────────────────────────────────
    static void saveMaterials(const vector<Material>& v) {
        vector<string> lines;
        for (auto& m : v)
            lines.push_back(to_string(m.id)+","+to_string(m.teacherId)+","+csvField(m.title)+","+csvField(m.subject)+","+csvField(m.contentPath)+","+csvField(m.className));
        writeCSV("data/materials.csv", lines);
    }
    static void loadMaterials(LMSManager& lms) {
        for (auto& r : readCSV("data/materials.csv")) {
            if (r.size() < 6) continue;
            lms.getMaterials().push_back({stoi(r[0]),stoi(r[1]),r[2],r[3],r[4],r[5]});
        }
    }

    // ── Books ─────────────────────────────────────────────────────────────────
    static void saveBooks(const vector<Book>& v) {
        vector<string> lines;
        for (auto& b : v)
            lines.push_back(to_string(b.id)+","+csvField(b.title)+","+csvField(b.author)+","+to_string(b.available)+","+to_string(b.issuedTo)+","+b.issueDate);
        writeCSV("data/books.csv", lines);
    }
    static void loadBooks(Library& lib) {
        for (auto& r : readCSV("data/books.csv")) {
            if (r.size() < 6) continue;
            lib.getBooks().push_back({stoi(r[0]),r[1],r[2],(bool)stoi(r[3]),stoi(r[4]),r[5]});
        }
    }

    // ── Exams ─────────────────────────────────────────────────────────────────
    static void saveExams(const vector<Exam>& v) {
        vector<string> lines;
        for (auto& e : v)
            lines.push_back(to_string(e.id)+","+csvField(e.title)+","+csvField(e.type)+","+csvField(e.className)+","+csvField(e.subject)+","+e.date+","+to_string(e.totalMarks));
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

    static void saveExamResults(const vector<ExamResult>& v) {
        vector<string> lines;
        for (auto& r : v)
            lines.push_back(to_string(r.id)+","+to_string(r.examId)+","+to_string(r.studentId)+","+csvField(r.studentName)+","+csvField(r.examTitle)+","+csvField(r.subject)+","+csvField(r.className)+","+
                            to_string(r.marksObtained)+","+to_string(r.totalMarks));
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
    static void saveCourses(const vector<Course>& v) {
        vector<string> lines;
        for (auto& c : v)
            lines.push_back(to_string(c.id)+","+csvField(c.code)+","+csvField(c.name)+","+csvField(c.subject)+","+csvField(c.className)+","+csvField(c.teacherName)+","+to_string(c.credits));
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
    static void saveNotifications(const vector<Notification>& v) {
        vector<string> lines;
        for (auto& n : v)
            lines.push_back(to_string(n.id)+","+to_string(n.userId)+","+
                            csvField(n.message)+","+n.date+","+csvField(n.category)+","+to_string(n.read));
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
    static void saveCalendar(const vector<CalendarEvent>& v) {
        vector<string> lines;
        for (auto& e : v)
            lines.push_back(to_string(e.id)+","+csvField(e.title)+","+e.date+","+e.endDate+","+csvField(e.type)+","+csvField(e.description));
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
