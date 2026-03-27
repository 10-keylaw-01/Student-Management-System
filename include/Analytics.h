#pragma once
#include "Attendance.h"
#include "Grade.h"
#include <iostream>
#include <iomanip>
#include <map>
#include <vector>
using namespace std;

class Analytics {
public:
    // ── Attendance Analytics ───────────────────────────────────────────────────
    static void attendanceReportByClass(const AttendanceManager& am, const string& cls) {
        map<int, pair<string,pair<int,int>>> data; // id -> {name, {present, total}}
        for (auto& e : am.getEntries()) {
            if (e.className == cls) {
                auto& d = data[e.studentId];
                d.first = e.studentName;
                d.second.second++;
                if (e.present) d.second.first++;
            }
        }
        if (data.empty()) { cout << "No attendance data for class " << cls << ".\n"; return; }
        cout << "\n--- Attendance Report: Class " << cls << " ---\n";
        cout << left << setw(20) << "Student" << setw(10) << "Present"
             << setw(10) << "Total" << "Percentage\n";
        cout << string(50, '-') << "\n";
        for (auto& [id, d] : data) {
            double pct = d.second.second ? (d.second.first * 100.0 / d.second.second) : 0;
            cout << left << setw(20) << d.first
                 << setw(10) << d.second.first
                 << setw(10) << d.second.second
                 << fixed << setprecision(1) << pct << "%"
                 << (pct < 75 ? " [LOW]" : "") << "\n";
        }
    }

    static void attendanceSummaryAllClasses(const AttendanceManager& am) {
        map<string, pair<int,int>> cls; // className -> {present, total}
        for (auto& e : am.getEntries()) {
            cls[e.className].second++;
            if (e.present) cls[e.className].first++;
        }
        if (cls.empty()) { cout << "No attendance data.\n"; return; }
        cout << "\n--- Attendance Summary by Class ---\n";
        for (auto& [c, p] : cls) {
            double pct = p.second ? (p.first * 100.0 / p.second) : 0;
            cout << "Class " << c << ": " << p.first << "/" << p.second
                 << " (" << fixed << setprecision(1) << pct << "%)\n";
        }
    }

    // ── Grade Analytics ────────────────────────────────────────────────────────
    static void gradeReportByClass(const GradeManager& gm,
                                   const vector<pair<int,string>>& studentList,
                                   const string& cls) {
        // studentList: vector of {studentId, className}
        cout << "\n--- Grade Report: Class " << cls << " ---\n";
        bool any = false;
        for (auto& [sid, scls] : studentList) {
            if (scls != cls) continue;
            double tot = 0, mx = 0; int cnt = 0;
            string sname;
            for (auto& e : gm.getEntries()) {
                if (e.studentId == sid) { tot += e.marks; mx += e.total; cnt++; sname = e.studentName; }
            }
            if (cnt) {
                double pct = (tot / mx) * 100;
                cout << left << setw(20) << sname
                     << fixed << setprecision(1) << pct << "% | GPA: "
                     << setprecision(2) << (pct/100*4.0) << "\n";
                any = true;
            }
        }
        if (!any) cout << "No grade data for class " << cls << ".\n";
    }

    static void subjectAverageByClass(const GradeManager& gm, const string& cls,
                                      const vector<pair<int,string>>& studentList) {
        // collect student IDs in this class
        vector<int> ids;
        for (auto& [sid, scls] : studentList) if (scls == cls) ids.push_back(sid);

        map<string, pair<double,int>> subj; // subject -> {total%, count}
        for (auto& e : gm.getEntries()) {
            for (int id : ids) {
                if (e.studentId == id) {
                    subj[e.subject].first += (e.marks / e.total) * 100;
                    subj[e.subject].second++;
                }
            }
        }
        if (subj.empty()) { cout << "No grade data.\n"; return; }
        cout << "\n--- Subject Averages: Class " << cls << " ---\n";
        for (auto& [s, p] : subj)
            cout << left << setw(20) << s
                 << fixed << setprecision(1) << (p.first / p.second) << "%\n";
    }
};
