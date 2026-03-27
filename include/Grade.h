#pragma once
#include <string>
#include <vector>
#include <iostream>
using namespace std;

struct GradeEntry {
    int studentId;
    string studentName, subject, term;
    double marks, total;
};

class GradeManager {
    vector<GradeEntry> entries;
public:
    void addGrade(int studentId, const string& name, const string& subject,
                  const string& term, double marks, double total) {
        entries.push_back({studentId, name, subject, term, marks, total});
        cout << "[Grade added: " << name << " | " << subject << " | " << marks << "/" << total << "]\n";
    }

    void viewReport(int studentId) const {
        double totalMarks = 0, totalMax = 0;
        bool found = false;
        for (auto& e : entries) {
            if (e.studentId == studentId) {
                double pct = (e.marks / e.total) * 100;
                cout << e.subject << " [" << e.term << "]: " << e.marks << "/" << e.total
                     << " (" << pct << "%)\n";
                totalMarks += e.marks; totalMax += e.total;
                found = true;
            }
        }
        if (found && totalMax > 0) {
            double overall = (totalMarks / totalMax) * 100;
            double gpa = (overall / 100) * 4.0;
            cout << "Overall: " << overall << "% | GPA: " << gpa << "\n";
        } else if (!found) cout << "No grades found.\n";
    }

    vector<GradeEntry>& getEntries() { return entries; }
    const vector<GradeEntry>& getEntries() const { return entries; }
};
