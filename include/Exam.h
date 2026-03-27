#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
using namespace std;

struct Exam {
    int id;
    string title, type, className, subject, date;
    double totalMarks;
};

struct ExamResult {
    int id, examId, studentId;
    string studentName, examTitle, subject, className;
    double marksObtained, totalMarks;
};

inline string gradeLetterFromPct(double pct) {
    if (pct >= 90) return "A+";
    if (pct >= 80) return "A";
    if (pct >= 70) return "B+";
    if (pct >= 60) return "B";
    if (pct >= 50) return "C";
    if (pct >= 40) return "D";
    return "F";
}

class ExamManager {
    vector<Exam>       exams;
    vector<ExamResult> results;
    int eId = 1, rId = 1;

public:
    void scheduleExam(const string& title, const string& type, const string& cls,
                      const string& subject, const string& date, double total) {
        // Conflict check: same class + date + subject
        for (auto& e : exams)
            if (e.className == cls && e.date == date && e.subject == subject) {
                cout << "[!] Conflict: " << cls << " already has a " << subject
                     << " exam on " << date << ".\n";
                return;
            }
        exams.push_back({eId++, title, type, cls, subject, date, total});
        cout << "[Exam scheduled: \"" << title << "\" | " << cls
             << " | " << subject << " | " << date << "]\n";
    }

    void enterResult(int examId, int studentId, const string& studentName, double marks) {
        for (auto& e : exams) {
            if (e.id == examId) {
                if (marks > e.totalMarks) {
                    cout << "[!] Marks exceed total (" << e.totalMarks << ").\n"; return;
                }
                results.push_back({rId++, examId, studentId, studentName,
                                   e.title, e.subject, e.className, marks, e.totalMarks});
                double pct = (marks / e.totalMarks) * 100;
                cout << "[Result entered: " << studentName << " | " << e.title
                     << " | " << marks << "/" << e.totalMarks
                     << " (" << fixed << setprecision(1) << pct << "%) | Grade: "
                     << gradeLetterFromPct(pct) << "]\n";
                return;
            }
        }
        cout << "Exam ID " << examId << " not found.\n";
    }

    void viewResultsByStudent(int studentId) const {
        double totalPct = 0; int count = 0;
        bool found = false;
        cout << "\n--- Exam Results ---\n";
        for (auto& r : results) {
            if (r.studentId == studentId) {
                double pct = (r.marksObtained / r.totalMarks) * 100;
                cout << r.examTitle << " [" << r.subject << "]: "
                     << r.marksObtained << "/" << r.totalMarks
                     << " (" << fixed << setprecision(1) << pct << "%)"
                     << " | Grade: " << gradeLetterFromPct(pct) << "\n";
                totalPct += pct; count++; found = true;
            }
        }
        if (found) {
            double avg = totalPct / count;
            double gpa = (avg / 100.0) * 4.0;
            cout << "Average: " << fixed << setprecision(1) << avg
                 << "% | GPA: " << setprecision(2) << gpa << "\n";
        } else cout << "No results found.\n";
    }

    void viewResultsByClass(const string& cls) const {
        bool found = false;
        cout << "\n--- Class Results: " << cls << " ---\n";
        for (auto& r : results) {
            if (r.className == cls) {
                double pct = (r.marksObtained / r.totalMarks) * 100;
                cout << r.studentName << " | " << r.examTitle
                     << " | " << r.marksObtained << "/" << r.totalMarks
                     << " (" << fixed << setprecision(1) << pct << "%)"
                     << " | " << gradeLetterFromPct(pct) << "\n";
                found = true;
            }
        }
        if (!found) cout << "No results for class " << cls << ".\n";
    }

    void listExams() const {
        if (exams.empty()) { cout << "No exams scheduled.\n"; return; }
        cout << "\n--- Scheduled Exams ---\n";
        for (auto& e : exams)
            cout << "ID:" << e.id << " | " << e.title << " | " << e.type
                 << " | Class:" << e.className << " | " << e.subject
                 << " | Date:" << e.date << " | Total:" << e.totalMarks << "\n";
    }

    void listExamsByClass(const string& cls) const {
        bool found = false;
        for (auto& e : exams)
            if (e.className == cls) {
                cout << "ID:" << e.id << " | " << e.title << " | " << e.subject
                     << " | " << e.date << " | Total:" << e.totalMarks << "\n";
                found = true;
            }
        if (!found) cout << "No exams for class " << cls << ".\n";
    }

    vector<Exam>&       getExams()   { return exams; }
    vector<ExamResult>& getResults() { return results; }
    void setNextIds(int e, int r)    { eId = e; rId = r; }
};
