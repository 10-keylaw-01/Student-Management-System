#pragma once
#include <string>
#include <vector>
#include <iostream>
using namespace std;

struct TimeSlot {
    string className, day, startTime, endTime, subject, teacherName;
};

class Timetable {
    vector<TimeSlot> slots;
public:
    void addSlot(const string& cls, const string& day, const string& start,
                 const string& end, const string& subject, const string& teacher) {
        slots.push_back({cls, day, start, end, subject, teacher});
        cout << "[Timetable slot added: " << cls << " | " << day << " | "
             << start << "-" << end << " | " << subject << "]\n";
    }

    void viewByClass(const string& cls) const {
        bool found = false;
        for (auto& s : slots) {
            if (s.className == cls) {
                cout << s.day << " " << s.startTime << "-" << s.endTime
                     << " | " << s.subject << " | " << s.teacherName << "\n";
                found = true;
            }
        }
        if (!found) cout << "No timetable found for class " << cls << ".\n";
    }

    vector<TimeSlot>& getSlots() { return slots; }
};
