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
using namespace std;

class FileManager {
    static void writeCSV(const string& path, const vector<string>& lines) {
        ofstream f(path);
        for (auto& l : lines) f << l << "\n";
    }

    static vector<vector<string>> readCSV(const string& path) {
        vector<vector<string>> result;
        ifstream f(path);
        string line;
        while (getline(f, line)) {
            if (line.empty()) continue;
            vector<string> row;
            stringstream ss(line);
            string cell;
            while (getline(ss, cell, ',')) row.push_back(cell);
            result.push_back(row);
        }
        return result;
    }

public:
    // --- Students ---
    static void saveStudents(const vector<Student>& students) {
        vector<string> lines;
        for (auto& s : students)
            lines.push_back(to_string(s.id)+","+s.username+","+s.password+","+
                            s.name+","+s.rollNumber+","+s.className+","+to_string(s.parentId));
        writeCSV("data/students.csv", lines);
    }

    static vector<Student> loadStudents() {
        vector<Student> students;
        for (auto& row : readCSV("data/students.csv")) {
            if (row.size() < 7) continue;
            students.emplace_back(stoi(row[0]), row[1], row[2], row[3], row[4], row[5], stoi(row[6]));
        }
        return students;
    }

    // --- Teachers ---
    static void saveTeachers(const vector<Teacher>& teachers) {
        vector<string> lines;
        for (auto& t : teachers)
            lines.push_back(to_string(t.id)+","+t.username+","+t.password+","+
                            t.name+","+t.subject+","+t.department);
        writeCSV("data/teachers.csv", lines);
    }

    static vector<Teacher> loadTeachers() {
        vector<Teacher> teachers;
        for (auto& row : readCSV("data/teachers.csv")) {
            if (row.size() < 6) continue;
            teachers.emplace_back(stoi(row[0]), row[1], row[2], row[3], row[4], row[5]);
        }
        return teachers;
    }

    // --- Admins ---
    static void saveAdmins(const vector<Admin>& admins) {
        vector<string> lines;
        for (auto& a : admins)
            lines.push_back(to_string(a.id)+","+a.username+","+a.password+","+a.name);
        writeCSV("data/admins.csv", lines);
    }

    static vector<Admin> loadAdmins() {
        vector<Admin> admins;
        for (auto& row : readCSV("data/admins.csv")) {
            if (row.size() < 4) continue;
            admins.emplace_back(stoi(row[0]), row[1], row[2], row[3]);
        }
        return admins;
    }

    // --- Parents ---
    static void saveParents(const vector<Parent>& parents) {
        vector<string> lines;
        for (auto& p : parents)
            lines.push_back(to_string(p.id)+","+p.username+","+p.password+","+
                            p.name+","+to_string(p.studentId));
        writeCSV("data/parents.csv", lines);
    }

    static vector<Parent> loadParents() {
        vector<Parent> parents;
        for (auto& row : readCSV("data/parents.csv")) {
            if (row.size() < 5) continue;
            parents.emplace_back(stoi(row[0]), row[1], row[2], row[3], stoi(row[4]));
        }
        return parents;
    }

    // --- Fees ---
    static void saveFees(const vector<FeeRecord>& records) {
        vector<string> lines;
        for (auto& r : records)
            lines.push_back(to_string(r.invoiceId)+","+to_string(r.studentId)+","+
                            r.studentName+","+r.dueDate+","+r.paidDate+","+
                            to_string(r.amount)+","+to_string(r.paid)+","+to_string(r.settled));
        writeCSV("data/fees.csv", lines);
    }

    static void loadFees(FeeManager& fm) {
        for (auto& row : readCSV("data/fees.csv")) {
            if (row.size() < 8) continue;
            FeeRecord r;
            r.invoiceId = stoi(row[0]); r.studentId = stoi(row[1]);
            r.studentName = row[2]; r.dueDate = row[3]; r.paidDate = row[4];
            r.amount = stod(row[5]); r.paid = stod(row[6]); r.settled = stoi(row[7]);
            fm.getRecords().push_back(r);
        }
    }

    // --- Attendance ---
    static void saveAttendance(const vector<AttendanceEntry>& entries) {
        vector<string> lines;
        for (auto& e : entries)
            lines.push_back(to_string(e.studentId)+","+e.studentName+","+
                            e.className+","+e.date+","+to_string(e.present));
        writeCSV("data/attendance.csv", lines);
    }

    static void loadAttendance(AttendanceManager& am) {
        for (auto& row : readCSV("data/attendance.csv")) {
            if (row.size() < 5) continue;
            am.getEntries().push_back({stoi(row[0]), row[1], row[2], row[3], (bool)stoi(row[4])});
        }
    }

    // --- Grades ---
    static void saveGrades(const vector<GradeEntry>& entries) {
        vector<string> lines;
        for (auto& e : entries)
            lines.push_back(to_string(e.studentId)+","+e.studentName+","+
                            e.subject+","+e.term+","+to_string(e.marks)+","+to_string(e.total));
        writeCSV("data/grades.csv", lines);
    }

    static void loadGrades(GradeManager& gm) {
        for (auto& row : readCSV("data/grades.csv")) {
            if (row.size() < 6) continue;
            gm.getEntries().push_back({stoi(row[0]), row[1], row[2], row[3], stod(row[4]), stod(row[5])});
        }
    }

    // --- Timetable ---
    static void saveTimetable(const vector<TimeSlot>& slots) {
        vector<string> lines;
        for (auto& s : slots)
            lines.push_back(s.className+","+s.day+","+s.startTime+","+s.endTime+","+s.subject+","+s.teacherName);
        writeCSV("data/timetable.csv", lines);
    }

    static void loadTimetable(Timetable& tt) {
        for (auto& row : readCSV("data/timetable.csv")) {
            if (row.size() < 6) continue;
            tt.getSlots().push_back({row[0], row[1], row[2], row[3], row[4], row[5]});
        }
    }

    // --- Announcements ---
    static void saveAnnouncements(const vector<Announcement>& anns) {
        vector<string> lines;
        for (auto& a : anns)
            lines.push_back(to_string(a.id)+","+a.title+","+a.content+","+a.date+","+
                            a.author+","+to_string((int)a.targetRole)+","+to_string(a.allRoles));
        writeCSV("data/announcements.csv", lines);
    }

    static void loadAnnouncements(AnnouncementManager& am) {
        for (auto& row : readCSV("data/announcements.csv")) {
            if (row.size() < 7) continue;
            am.getAll().push_back({stoi(row[0]), row[1], row[2], row[3], row[4],
                                   (Role)stoi(row[5]), (bool)stoi(row[6])});
        }
    }

    // --- Assignments ---
    static void saveAssignments(const vector<Assignment>& assignments) {
        vector<string> lines;
        for (auto& a : assignments)
            lines.push_back(to_string(a.id)+","+to_string(a.teacherId)+","+
                            a.title+","+a.subject+","+a.dueDate+","+a.fileRef+","+a.className);
        writeCSV("data/assignments.csv", lines);
    }

    static void loadAssignments(LMSManager& lms) {
        for (auto& row : readCSV("data/assignments.csv")) {
            if (row.size() < 7) continue;
            lms.getAssignments().push_back({stoi(row[0]), stoi(row[1]), row[2], row[3], row[4], row[5], row[6]});
        }
    }

    // --- Materials ---
    static void saveMaterials(const vector<Material>& materials) {
        vector<string> lines;
        for (auto& m : materials)
            lines.push_back(to_string(m.id)+","+to_string(m.teacherId)+","+
                            m.title+","+m.subject+","+m.contentPath+","+m.className);
        writeCSV("data/materials.csv", lines);
    }

    static void loadMaterials(LMSManager& lms) {
        for (auto& row : readCSV("data/materials.csv")) {
            if (row.size() < 6) continue;
            lms.getMaterials().push_back({stoi(row[0]), stoi(row[1]), row[2], row[3], row[4], row[5]});
        }
    }

    // --- Books ---
    static void saveBooks(const vector<Book>& books) {
        vector<string> lines;
        for (auto& b : books)
            lines.push_back(to_string(b.id)+","+b.title+","+b.author+","+
                            to_string(b.available)+","+to_string(b.issuedTo)+","+b.issueDate);
        writeCSV("data/books.csv", lines);
    }

    static void loadBooks(Library& lib) {
        for (auto& row : readCSV("data/books.csv")) {
            if (row.size() < 6) continue;
            lib.getBooks().push_back({stoi(row[0]), row[1], row[2],
                                      (bool)stoi(row[3]), stoi(row[4]), row[5]});
        }
    }
};
