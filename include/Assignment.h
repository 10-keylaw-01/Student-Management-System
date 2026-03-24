#pragma once
#include <string>
#include <vector>
#include <iostream>
using namespace std;

struct Assignment {
    int id, teacherId;
    string title, subject, dueDate, fileRef, className;
};

struct Material {
    int id, teacherId;
    string title, subject, contentPath, className;
};

class LMSManager {
    vector<Assignment> assignments;
    vector<Material> materials;
    int aId = 1, mId = 1;
public:
    void uploadAssignment(int teacherId, const string& title, const string& subject,
                          const string& dueDate, const string& fileRef, const string& cls) {
        assignments.push_back({aId++, teacherId, title, subject, dueDate, fileRef, cls});
        cout << "[Assignment uploaded: \"" << title << "\" for class " << cls
             << " | Due: " << dueDate << "]\n";
    }

    void uploadMaterial(int teacherId, const string& title, const string& subject,
                        const string& path, const string& cls) {
        materials.push_back({mId++, teacherId, title, subject, path, cls});
        cout << "[Material uploaded: \"" << title << "\" for class " << cls << "]\n";
    }

    void viewAssignments(const string& cls) const {
        bool found = false;
        for (auto& a : assignments) {
            if (a.className == cls) {
                cout << "Assignment #" << a.id << ": " << a.title << " | " << a.subject
                     << " | Due: " << a.dueDate << " | File: " << a.fileRef << "\n";
                found = true;
            }
        }
        if (!found) cout << "No assignments found.\n";
    }

    void viewMaterials(const string& cls) const {
        bool found = false;
        for (auto& m : materials) {
            if (m.className == cls) {
                cout << "Material #" << m.id << ": " << m.title << " | " << m.subject
                     << " | Path: " << m.contentPath << "\n";
                found = true;
            }
        }
        if (!found) cout << "No materials found.\n";
    }

    vector<Assignment>& getAssignments() { return assignments; }
    vector<Material>& getMaterials() { return materials; }
};
