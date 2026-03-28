#pragma once
#include <string>
#include <iostream>

// User role enumeration
enum class Role { Admin, Teacher, Parent, Student };

/// Base class for all users in the system
class User {
public:
    int id = 0;
    std::string username, password, name;
    Role role = Role::Student;

    User() = default;
    User(int id, const std::string& username, const std::string& password, 
         const std::string& name, Role role)
        : id(id), username(username), password(password), name(name), role(role) {}

    virtual void viewDashboard() const = 0;
    virtual ~User() = default;

    /// Convert role enum to string representation
    std::string roleStr() const {
        switch(role) {
            case Role::Admin:   return "Admin";
            case Role::Teacher: return "Teacher";
            case Role::Parent:  return "Parent";
            case Role::Student: return "Student";
        }
        return "";
    }
};
