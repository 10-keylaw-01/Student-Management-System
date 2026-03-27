#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>

/// Course structure
struct Course {
    int id;
    std::string code, name, subject, className, teacherName;
    int credits;
};

/// Manages courses offered by the school
class CourseManager {
    std::vector<Course> courses;
    int nextId = 1;

public:
    /// Add a new course (prevents duplicate course codes)
    void addCourse(const std::string& code, const std::string& name, const std::string& subject,
                   const std::string& cls, const std::string& teacher, int credits) {
        // prevent duplicate code
        for (const auto& c : courses)
            if (c.code == code) { 
                std::cout << "Course code \"" << code << "\" already exists.\n"; 
                return; 
            }
        courses.push_back({nextId++, code, name, subject, cls, teacher, credits});
        std::cout << "[Course added: " << code << " - " << name << " | Class: " << cls << "]\n";
    }

    /// Modify existing course details
    void modifyCourse(int id, const std::string& name, const std::string& teacher, int credits) {
        for (auto& c : courses) {
            if (c.id == id) {
                if (!name.empty())    c.name        = name;
                if (!teacher.empty()) c.teacherName = teacher;
                if (credits > 0)      c.credits     = credits;
                std::cout << "[Course #" << id << " updated.]\n";
                return;
            }
        }
        std::cout << "Course not found.\n";
    }

    /// Delete a course
    void deleteCourse(int id) {
        for (auto it = courses.begin(); it != courses.end(); ++it) {
            if (it->id == id) {
                std::cout << "[Course \"" << it->name << "\" deleted.]\n";
                courses.erase(it); 
                return;
            }
        }
        std::cout << "Course not found.\n";
    }

    /// List all courses
    void listAll() const {
        if (courses.empty()) { std::cout << "No courses.\n"; return; }
        std::cout << "\n--- All Courses ---\n";
        std::cout << std::left << std::setw(4) << "ID" << std::setw(10) << "Code"
             << std::setw(25) << "Name" << std::setw(15) << "Subject"
             << std::setw(8) << "Class" << std::setw(20) << "Teacher" << "Credits\n";
        std::cout << std::string(90, '-') << "\n";
        for (const auto& c : courses)
            std::cout << std::left << std::setw(4) << c.id << std::setw(10) << c.code
                 << std::setw(25) << c.name << std::setw(15) << c.subject
                 << std::setw(8) << c.className << std::setw(20) << c.teacherName
                 << c.credits << "\n";
    }

    /// List courses for a specific class
    void listByClass(const std::string& cls) const {
        bool found = false;
        std::cout << "\n--- Courses for Class " << cls << " ---\n";
        for (const auto& c : courses) {
            if (c.className == cls) {
                std::cout << c.code << " | " << c.name << " | " << c.subject
                     << " | Teacher: " << c.teacherName
                     << " | Credits: " << c.credits << "\n";
                found = true;
            }
        }
        if (!found) std::cout << "No courses found for class " << cls << ".\n";
    }

    std::vector<Course>& getCourses() { return courses; }
    void setNextId(int id) { nextId = id; }
};
