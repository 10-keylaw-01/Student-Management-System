#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>

/// Calendar event structure
struct CalendarEvent {
    int id;
    std::string title, date, endDate, type, description;
    // type: Holiday | Exam | Meeting | Sports | Academic | Other
};

/// Manages academic calendar events
class AcademicCalendar {
    std::vector<CalendarEvent> events;
    int nextId = 1;

public:
    /// Add an event to the academic calendar
    void addEvent(const std::string& title, const std::string& date, const std::string& endDate,
                  const std::string& type, const std::string& desc) {
        events.push_back({nextId++, title, date, endDate, type, desc});
        std::cout << "[Calendar: \"" << title << "\" added on " << date << " (" << type << ")]\n";
    }

    /// Remove an event from the calendar
    void removeEvent(int id) {
        for (auto it = events.begin(); it != events.end(); ++it) {
            if (it->id == id) { 
                std::cout << "[Event \"" << it->title << "\" removed.]\n"; 
                events.erase(it); 
                return; 
            }
        }
        std::cout << "Event not found.\n";
    }

    /// List all calendar events
    void listAll() const {
        if (events.empty()) { std::cout << "No events.\n"; return; }
        std::cout << "\n--- Academic Calendar ---\n";
        std::cout << std::left << std::setw(4) << "ID" << std::setw(25) << "Title"
             << std::setw(12) << "Date" << std::setw(12) << "End Date"
             << std::setw(12) << "Type" << "Description\n";
        std::cout << std::string(80, '-') << "\n";
        for (const auto& e : events)
            std::cout << std::left << std::setw(4) << e.id << std::setw(25) << e.title
                 << std::setw(12) << e.date << std::setw(12) << e.endDate
                 << std::setw(12) << e.type << e.description << "\n";
    }

    /// List events of a specific type
    void listByType(const std::string& type) const {
        bool found = false;
        for (const auto& e : events)
            if (e.type == type) {
                std::cout << e.date << " - " << e.title << ": " << e.description << "\n";
                found = true;
            }
        if (!found) std::cout << "No events of type \"" << type << "\".\n";
    }

    /// List upcoming events from a given date
    void listUpcoming(const std::string& fromDate) const {
        bool found = false;
        std::cout << "\n--- Upcoming Events (from " << fromDate << ") ---\n";
        for (const auto& e : events)
            if (e.date >= fromDate) {
                std::cout << "[" << e.type << "] " << e.date << " | " << e.title
                     << " - " << e.description << "\n";
                found = true;
            }
        if (!found) std::cout << "No upcoming events.\n";
    }

    std::vector<CalendarEvent>& getEvents() { return events; }
    void setNextId(int id) { nextId = id; }
};
