#pragma once
#include <string>
#include <vector>
#include <iostream>

/// Time slot structure for timetable
struct TimeSlot {
    std::string className, day, startTime, endTime, subject, teacherName;
};

/// Manages class timetables
class Timetable {
    std::vector<TimeSlot> slots;
public:
    /// Add a time slot to the timetable
    void addSlot(const std::string& cls, const std::string& day, const std::string& start,
                 const std::string& end, const std::string& subject, const std::string& teacher) {
        slots.push_back({cls, day, start, end, subject, teacher});
        std::cout << "[Timetable slot added: " << cls << " | " << day << " | "
             << start << "-" << end << " | " << subject << "]\n";
    }

    /// View timetable for a specific class
    void viewByClass(const std::string& cls) const {
        bool found = false;
        for (const auto& s : slots) {
            if (s.className == cls) {
                std::cout << s.day << " " << s.startTime << "-" << s.endTime
                     << " | " << s.subject << " | " << s.teacherName << "\n";
                found = true;
            }
        }
        if (!found) std::cout << "No timetable found for class " << cls << ".\n";
    }

    std::vector<TimeSlot>& getSlots() { return slots; }
};
