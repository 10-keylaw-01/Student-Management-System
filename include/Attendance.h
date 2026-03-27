#pragma once
#include <string>
#include <vector>
#include <iostream>

/// Attendance entry structure
struct AttendanceEntry {
    int studentId;
    std::string studentName, className, date;
    bool present;
};

/// Manages attendance records for students
class AttendanceManager {
    std::vector<AttendanceEntry> entries;
public:
    /// Mark attendance for a student on a specific date (prevents duplicates)
    void mark(int studentId, const std::string& name, const std::string& cls,
              const std::string& date, bool present) {
        // Check for existing entry on same date for same student
        for (auto& e : entries) {
            if (e.studentId == studentId && e.date == date && e.className == cls) {
                e.present = present;
                std::cout << "[Attendance updated: " << name << " on " << date
                     << " - " << (present ? "Present" : "Absent") << "]\n";
                return;
            }
        }
        // Add new entry if none exists
        entries.push_back({studentId, name, cls, date, present});
        std::cout << "[Attendance marked: " << name << " on " << date
             << " - " << (present ? "Present" : "Absent") << "]\n";
    }

    /// View attendance record for a specific student (with percentage calculation)
    void viewByStudent(int studentId) const {
        int total = 0, present = 0;
        for (const auto& e : entries) {
            if (e.studentId == studentId) {
                std::cout << e.date << " - " << (e.present ? "Present" : "Absent") << "\n";
                total++; 
                if (e.present) present++;
            }
        }
        if (total > 0) {
            double percentage = (present * 100.0) / total;
            std::cout << "Attendance: " << present << "/" << total
                        << " (" << percentage << "%)\n";
        }
        else std::cout << "No records found.\n";
    }

    /// View attendance by class (optional date filter)
    void viewByClass(const std::string& cls, const std::string& date = "") const {
        bool found = false;
        for (auto& e : entries) {
            if (e.className == cls && (date.empty() || e.date == date)) {
                std::cout << e.studentName << " - " << (e.present ? "Present" : "Absent") << "\n";
                found = true;
            }
        }
        if (!found) std::cout << "No records found.\n";
    }

    std::vector<AttendanceEntry>& getEntries() { return entries; }
    const std::vector<AttendanceEntry>& getEntries() const { return entries; }
};
