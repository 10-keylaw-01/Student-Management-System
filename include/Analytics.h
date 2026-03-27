#pragma once
#include "Attendance.h"
#include "Grade.h"
#include <iostream>
#include <iomanip>
#include <map>
#include <vector>

/// Provides analytics and reporting on attendance and grades
class Analytics {
public:
    /// Generate attendance report for a specific class
    static void attendanceReportByClass(const AttendanceManager& am, const std::string& cls) {
        std::map<int, std::pair<std::string,std::pair<int,int>>> data; // id -> {name, {present, total}}
        for (const auto& e : am.getEntries()) {
            if (e.className == cls) {
                auto& d = data[e.studentId];
                d.first = e.studentName;
                d.second.second++;
                if (e.present) d.second.first++;
            }
        }
        if (data.empty()) { 
            std::cout << "No attendance data for class " << cls << ".\n"; 
            return; 
        }
        std::cout << "\n--- Attendance Report: Class " << cls << " ---\n";
        std::cout << std::left << std::setw(20) << "Student" << std::setw(10) << "Present"
             << std::setw(10) << "Total" << "Percentage\n";
        std::cout << std::string(50, '-') << "\n";
        for (const auto& [id, d] : data) {
            double pct = d.second.second ? (d.second.first * 100.0 / d.second.second) : 0;
            std::cout << std::left << std::setw(20) << d.first
                 << std::setw(10) << d.second.first
                 << std::setw(10) << d.second.second
                 << std::fixed << std::setprecision(1) << pct << "%"
                 << (pct < 75 ? " [LOW]" : "") << "\n";
        }
    }

    /// Generate attendance summary for all classes
    static void attendanceSummaryAllClasses(const AttendanceManager& am) {
        std::map<std::string, std::pair<int,int>> cls; // className -> {present, total}
        for (const auto& e : am.getEntries()) {
            cls[e.className].second++;
            if (e.present) cls[e.className].first++;
        }
        if (cls.empty()) { 
            std::cout << "No attendance data.\n"; 
            return; 
        }
        std::cout << "\n--- Attendance Summary by Class ---\n";
        for (const auto& [c, p] : cls) {
            double pct = p.second ? (p.first * 100.0 / p.second) : 0;
            std::cout << "Class " << c << ": " << p.first << "/" << p.second
                 << " (" << std::fixed << std::setprecision(1) << pct << "%)\n";
        }
    }

    /// Generate grade report for a specific class
    static void gradeReportByClass(const GradeManager& gm,
                                   const std::vector<std::pair<int,std::string>>& studentList,
                                   const std::string& cls) {
        // studentList: vector of {studentId, className}
        std::cout << "\n--- Grade Report: Class " << cls << " ---\n";
        bool any = false;
        for (const auto& [sid, scls] : studentList) {
            if (scls != cls) continue;
            double tot = 0, mx = 0; int cnt = 0;
            std::string sname;
            for (const auto& e : gm.getEntries()) {
                if (e.studentId == sid) { tot += e.marks; mx += e.total; cnt++; sname = e.studentName; }
            }
            if (cnt && mx > 0) {
                double pct = (tot / mx) * 100;
                std::cout << std::left << std::setw(20) << sname
                     << std::fixed << std::setprecision(1) << pct << "% | GPA: "
                     << std::setprecision(2) << (pct/100*4.0) << "\n";
                any = true;
            }
        }
        if (!any) std::cout << "No grade data for class " << cls << ".\n";
    }

    /// Calculate subject averages for a class
    static void subjectAverageByClass(const GradeManager& gm, const std::string& cls,
                                      const std::vector<std::pair<int,std::string>>& studentList) {
        // collect student IDs in this class
        std::vector<int> ids;
        for (const auto& [sid, scls] : studentList) 
            if (scls == cls) ids.push_back(sid);

        std::map<std::string, std::pair<double,int>> subj; // subject -> {total%, count}
        for (const auto& e : gm.getEntries()) {
            for (int id : ids) {
                if (e.studentId == id && e.total > 0) {
                    subj[e.subject].first += (e.marks / e.total) * 100;
                    subj[e.subject].second++;
                }
            }
        }
        if (subj.empty()) { std::cout << "No grade data.\n"; return; }
        std::cout << "\n--- Subject Averages: Class " << cls << " ---\n";
        for (const auto& [s, p] : subj)
            std::cout << std::left << std::setw(20) << s
                 << std::fixed << std::setprecision(1) << (p.first / p.second) << "%\n";
    }
};
