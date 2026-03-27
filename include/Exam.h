#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>

/// Exam structure
struct Exam {
    int id;
    std::string title, type, className, subject, date;
    double totalMarks;
};

/// Exam result structure
struct ExamResult {
    int id, examId, studentId;
    std::string studentName, examTitle, subject, className;
    double marksObtained, totalMarks;
};

/// Convert percentage to letter grade
inline std::string gradeLetterFromPct(double pct) {
    if (pct >= 90) return "A+";
    if (pct >= 80) return "A";
    if (pct >= 70) return "B+";
    if (pct >= 60) return "B";
    if (pct >= 50) return "C";
    if (pct >= 40) return "D";
    return "F";
}

/// Manages exam scheduling and results tracking
class ExamManager {
    std::vector<Exam>       exams;
    std::vector<ExamResult> results;
    int eId = 1, rId = 1;

public:
    /// Schedule a new exam (prevents scheduling conflicts)
    void scheduleExam(const std::string& title, const std::string& type, const std::string& cls,
                      const std::string& subject, const std::string& date, double total) {
        // Conflict check: same class + date + subject
        for (const auto& e : exams)
            if (e.className == cls && e.date == date && e.subject == subject) {
                std::cout << "[!] Conflict: " << cls << " already has a " << subject
                     << " exam on " << date << ".\n";
                return;
            }
        exams.push_back({eId++, title, type, cls, subject, date, total});
        std::cout << "[Exam scheduled: \"" << title << "\" | " << cls
             << " | " << subject << " | " << date << "]\n";
    }

    /// Enter result for an exam
    void enterResult(int examId, int studentId, const std::string& studentName, double marks) {
        // Check for duplicate result
        for (const auto& r : results) {
            if (r.examId == examId && r.studentId == studentId) {
                std::cout << "[!] Result already exists for this student-exam combination.\n";
                return;
            }
        }
        
        for (const auto& e : exams) {
            if (e.id == examId) {
                if (marks > e.totalMarks) {
                    std::cout << "[!] Marks exceed total (" << e.totalMarks << ").\n"; 
                    return;
                }
                results.push_back({rId++, examId, studentId, studentName,
                                   e.title, e.subject, e.className, marks, e.totalMarks});
                if (e.totalMarks > 0) {
                    double pct = (marks / e.totalMarks) * 100;
                    std::cout << "[Result entered: " << studentName << " | " << e.title
                         << " | " << marks << "/" << e.totalMarks
                         << " (" << std::fixed << std::setprecision(1) << pct << "%) | Grade: "
                         << gradeLetterFromPct(pct) << "]\n";
                } else {
                    std::cout << "[Result entered: " << studentName << " | " << e.title << "]\n";
                }
                return;
            }
        }
        std::cout << "Exam ID " << examId << " not found.\n";
    }

    /// View results for a specific student
    void viewResultsByStudent(int studentId) const {
        double totalPct = 0; int count = 0;
        bool found = false;
        std::cout << "\n--- Exam Results ---\n";
        for (const auto& r : results) {
            if (r.studentId == studentId) {
                if (r.totalMarks > 0) {
                    double pct = (r.marksObtained / r.totalMarks) * 100;
                    std::cout << r.examTitle << " [" << r.subject << "]: "
                         << r.marksObtained << "/" << r.totalMarks
                         << " (" << std::fixed << std::setprecision(1) << pct << "%)"
                         << " | Grade: " << gradeLetterFromPct(pct) << "\n";
                    totalPct += pct; count++;
                } else {
                    std::cout << r.examTitle << " [" << r.subject << "]: "
                         << r.marksObtained << "/" << r.totalMarks << "\n";
                }
                found = true;
            }
        }
        if (found) {
            if (count > 0) {
                double avg = totalPct / count;
                double gpa = (avg / 100.0) * 4.0;
                std::cout << "Average: " << std::fixed << std::setprecision(1) << avg
                     << "% | GPA: " << std::setprecision(2) << gpa << "\n";
            }
        } else std::cout << "No results found.\n";
    }

    /// View exam results for a class
    void viewResultsByClass(const std::string& cls) const {
        bool found = false;
        std::cout << "\n--- Class Results: " << cls << " ---\n";
        for (const auto& r : results) {
            if (r.className == cls) {
                if (r.totalMarks > 0) {
                    double pct = (r.marksObtained / r.totalMarks) * 100;
                    std::cout << r.studentName << " | " << r.examTitle
                         << " | " << r.marksObtained << "/" << r.totalMarks
                         << " (" << std::fixed << std::setprecision(1) << pct << "%)"
                         << " | " << gradeLetterFromPct(pct) << "\n";
                } else {
                    std::cout << r.studentName << " | " << r.examTitle
                         << " | " << r.marksObtained << "/" << r.totalMarks << "\n";
                }
                found = true;
            }
        }
        if (!found) std::cout << "No results for class " << cls << ".\n";
    }

    /// List all scheduled exams
    void listExams() const {
        if (exams.empty()) { std::cout << "No exams scheduled.\n"; return; }
        std::cout << "\n--- Scheduled Exams ---\n";
        for (const auto& e : exams)
            std::cout << "ID:" << e.id << " | " << e.title << " | " << e.type
                 << " | Class:" << e.className << " | " << e.subject
                 << " | Date:" << e.date << " | Total:" << e.totalMarks << "\n";
    }

    /// List exams for a specific class
    void listExamsByClass(const std::string& cls) const {
        bool found = false;
        for (const auto& e : exams)
            if (e.className == cls) {
                std::cout << "ID:" << e.id << " | " << e.title << " | " << e.subject
                     << " | " << e.date << " | Total:" << e.totalMarks << "\n";
                found = true;
            }
        if (!found) std::cout << "No exams for class " << cls << ".\n";
    }

    std::vector<Exam>&       getExams()   { return exams; }
    std::vector<ExamResult>& getResults() { return results; }
    void setNextIds(int e, int r)         { eId = e; rId = r; }
};
