#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
using namespace std;

struct Course {
    int id;
    string code, name, subject, className, teacherName;
    int credits;
};

class CourseManager {
    vector<Course> courses;
    int nextId = 1;

public:
    void addCourse(const string& code, const string& name, const string& subject,
                   const string& cls, const string& teacher, int credits) {
        // prevent duplicate code
        for (auto& c : courses)
            if (c.code == code) { cout << "Course code \"" << code << "\" already exists.\n"; return; }
        courses.push_back({nextId++, code, name, subject, cls, teacher, credits});
        cout << "[Course added: " << code << " - " << name << " | Class: " << cls << "]\n";
    }

    void modifyCourse(int id, const string& name, const string& teacher, int credits) {
        for (auto& c : courses) {
            if (c.id == id) {
                if (!name.empty())    c.name        = name;
                if (!teacher.empty()) c.teacherName = teacher;
                if (credits > 0)      c.credits     = credits;
                cout << "[Course #" << id << " updated.]\n";
                return;
            }
        }
        cout << "Course not found.\n";
    }

    void deleteCourse(int id) {
        for (auto it = courses.begin(); it != courses.end(); ++it) {
            if (it->id == id) {
                cout << "[Course \"" << it->name << "\" deleted.]\n";
                courses.erase(it); return;
            }
        }
        cout << "Course not found.\n";
    }

    void listAll() const {
        if (courses.empty()) { cout << "No courses.\n"; return; }
        cout << "\n--- All Courses ---\n";
        cout << left << setw(4) << "ID" << setw(10) << "Code"
             << setw(25) << "Name" << setw(15) << "Subject"
             << setw(8) << "Class" << setw(20) << "Teacher" << "Credits\n";
        cout << string(90, '-') << "\n";
        for (auto& c : courses)
            cout << left << setw(4) << c.id << setw(10) << c.code
                 << setw(25) << c.name << setw(15) << c.subject
                 << setw(8) << c.className << setw(20) << c.teacherName
                 << c.credits << "\n";
    }

    void listByClass(const string& cls) const {
        bool found = false;
        cout << "\n--- Courses for Class " << cls << " ---\n";
        for (auto& c : courses) {
            if (c.className == cls) {
                cout << c.code << " | " << c.name << " | " << c.subject
                     << " | Teacher: " << c.teacherName
                     << " | Credits: " << c.credits << "\n";
                found = true;
            }
        }
        if (!found) cout << "No courses found for class " << cls << ".\n";
    }

    vector<Course>& getCourses() { return courses; }
    void setNextId(int id) { nextId = id; }
};
