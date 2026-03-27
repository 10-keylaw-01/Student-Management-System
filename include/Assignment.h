#pragma once
#include <string>
#include <vector>
#include <iostream>

/// Assignment structure
struct Assignment {
    int id, teacherId;
    std::string title, subject, dueDate, fileRef, className;
};

/// Learning material structure
struct Material {
    int id, teacherId;
    std::string title, subject, contentPath, className;
};

/// Manages assignments and learning materials
class LMSManager {
    std::vector<Assignment> assignments;
    std::vector<Material> materials;
    int aId = 1, mId = 1;
public:
    void setNextIds(int a, int m) { aId = a; mId = m; }
    
    /// Upload assignment for a specific class
    void uploadAssignment(int teacherId, const std::string& title, const std::string& subject,
                          const std::string& dueDate, const std::string& fileRef, const std::string& cls) {
        assignments.push_back({aId++, teacherId, title, subject, dueDate, fileRef, cls});
        std::cout << "[Assignment uploaded: \"" << title << "\" for class " << cls
             << " | Due: " << dueDate << "]\n";
    }

    /// Upload learning material for a specific class
    void uploadMaterial(int teacherId, const std::string& title, const std::string& subject,
                        const std::string& path, const std::string& cls) {
        materials.push_back({mId++, teacherId, title, subject, path, cls});
        std::cout << "[Material uploaded: \"" << title << "\" for class " << cls << "]\n";
    }

    /// View assignments for a specific class
    void viewAssignments(const std::string& cls) const {
        bool found = false;
        for (const auto& a : assignments) {
            if (a.className == cls) {
                std::cout << "Assignment #" << a.id << ": " << a.title << " | " << a.subject
                     << " | Due: " << a.dueDate << " | File: " << a.fileRef << "\n";
                found = true;
            }
        }
        if (!found) std::cout << "No assignments found.\n";
    }

    /// View materials for a specific class
    void viewMaterials(const std::string& cls) const {
        bool found = false;
        for (const auto& m : materials) {
            if (m.className == cls) {
                std::cout << "Material #" << m.id << ": " << m.title << " | " << m.subject
                     << " | Path: " << m.contentPath << "\n";
                found = true;
            }
        }
        if (!found) std::cout << "No materials found.\n";
    }

    std::vector<Assignment>& getAssignments() { return assignments; }
    std::vector<Material>& getMaterials() { return materials; }
};
