#pragma once
#include <string>
using namespace std;

enum class Role { Admin, Teacher, Parent, Student };

class User {
public:
    int id;
    string username, password, name;
    Role role;

    User() = default;
    User(int id, string username, string password, string name, Role role)
        : id(id), username(username), password(password), name(name), role(role) {}

    virtual void viewDashboard() const = 0;
    virtual ~User() = default;

    string roleStr() const {
        switch(role) {
            case Role::Admin:   return "Admin";
            case Role::Teacher: return "Teacher";
            case Role::Parent:  return "Parent";
            case Role::Student: return "Student";
        }
        return "";
    }
};
