#pragma once
#include <string>
#include <vector>
#include <iostream>
using namespace std;

struct AttendanceEntry {
    int studentId;
    string studentName, className, date;
    bool present;
};

class AttendanceManager {
    vector<AttendanceEntry> entries;
public:
    void mark(int studentId, const string& name, const string& cls,
              const string& date, bool present) {
        entries.push_back({studentId, name, cls, date, present});
        cout << "[Attendance marked: " << name << " on " << date
             << " - " << (present ? "Present" : "Absent") << "]\n";
    }

    void viewByStudent(int studentId) const {
        int total = 0, present = 0;
        for (auto& e : entries) {
            if (e.studentId == studentId) {
                cout << e.date << " - " << (e.present ? "Present" : "Absent") << "\n";
                total++; if (e.present) present++;
            }
        }
        if (total) cout << "Attendance: " << present << "/" << total
                        << " (" << (present*100/total) << "%)\n";
        else cout << "No records found.\n";
    }

    void viewByClass(const string& cls, const string& date) const {
        bool found = false;
        for (auto& e : entries) {
            if (e.className == cls && (date.empty() || e.date == date)) {
                cout << e.studentName << " - " << (e.present ? "Present" : "Absent") << "\n";
                found = true;
            }
        }
        if (!found) cout << "No records found.\n";
    }

    vector<AttendanceEntry>& getEntries() { return entries; }
};
