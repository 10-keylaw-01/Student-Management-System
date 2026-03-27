#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>

/// Grade entry structure for student marks
struct GradeEntry {
    int studentId;
    std::string studentName, subject, term;
    double marks, total;
};

/// Manages student grades
class GradeManager {
    std::vector<GradeEntry> entries;
public:
    /// Add or update grade for a student (prevents duplicates)
    void addGrade(int studentId, const std::string& name, const std::string& subject,
                  const std::string& term, double marks, double total) {
        // Check if grade for this student+subject+term exists
        for (auto& e : entries) {
            if (e.studentId == studentId && e.subject == subject && e.term == term) {
                e.marks = marks;
                e.total = total;
                std::cout << "[Grade updated: " << name << " | " << subject << " | "
                     << marks << "/" << total << "]\n";
                return;
            }
        }
        // Add new grade if doesn't exist
        entries.push_back({studentId, name, subject, term, marks, total});
        std::cout << "[Grade added: " << name << " | " << subject << " | "
             << marks << "/" << total << "]\n";
    }

    /// View grade report for a student (guards against division by zero)
    void viewReport(int studentId) const {
        double totalMarks = 0, totalMax = 0;
        bool found = false;
        for (const auto& e : entries) {
            if (e.studentId == studentId) {
                // Guard against division by zero
                if (e.total > 0) {
                    double pct = (e.marks / e.total) * 100;
                    std::cout << e.subject << " [" << e.term << "]: " << e.marks << "/" << e.total
                         << " (" << std::fixed << std::setprecision(1) << pct << "%)\n";
                } else {
                    std::cout << e.subject << " [" << e.term << "]: " << e.marks << "/" << e.total
                         << " (No total marks)\n";
                }
                totalMarks += e.marks; 
                totalMax += e.total;
                found = true;
            }
        }
        if (found && totalMax > 0) {
            double overall = (totalMarks / totalMax) * 100;
            double gpa = (overall / 100) * 4.0;
            std::cout << "Overall: " << std::fixed << std::setprecision(1) << overall 
                 << "% | GPA: " << std::setprecision(2) << gpa << "\n";
        } else if (!found) std::cout << "No grades found.\n";
    }

    std::vector<GradeEntry>& getEntries() { return entries; }
    const std::vector<GradeEntry>& getEntries() const { return entries; }
};
