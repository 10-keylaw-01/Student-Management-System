#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
using namespace std;

struct CalendarEvent {
    int id;
    string title, date, endDate, type, description;
    // type: Holiday | Exam | Meeting | Sports | Academic | Other
};

class AcademicCalendar {
    vector<CalendarEvent> events;
    int nextId = 1;

public:
    void addEvent(const string& title, const string& date, const string& endDate,
                  const string& type, const string& desc) {
        events.push_back({nextId++, title, date, endDate, type, desc});
        cout << "[Calendar: \"" << title << "\" added on " << date << " (" << type << ")]\n";
    }

    void removeEvent(int id) {
        for (auto it = events.begin(); it != events.end(); ++it) {
            if (it->id == id) { cout << "[Event \"" << it->title << "\" removed.]\n"; events.erase(it); return; }
        }
        cout << "Event not found.\n";
    }

    void listAll() const {
        if (events.empty()) { cout << "No events.\n"; return; }
        cout << "\n--- Academic Calendar ---\n";
        cout << left << setw(4) << "ID" << setw(25) << "Title"
             << setw(12) << "Date" << setw(12) << "End Date"
             << setw(12) << "Type" << "Description\n";
        cout << string(80, '-') << "\n";
        for (auto& e : events)
            cout << left << setw(4) << e.id << setw(25) << e.title
                 << setw(12) << e.date << setw(12) << e.endDate
                 << setw(12) << e.type << e.description << "\n";
    }

    void listByType(const string& type) const {
        bool found = false;
        for (auto& e : events)
            if (e.type == type) {
                cout << e.date << " - " << e.title << ": " << e.description << "\n";
                found = true;
            }
        if (!found) cout << "No events of type \"" << type << "\".\n";
    }

    void listUpcoming(const string& fromDate) const {
        bool found = false;
        cout << "\n--- Upcoming Events (from " << fromDate << ") ---\n";
        for (auto& e : events)
            if (e.date >= fromDate) {
                cout << "[" << e.type << "] " << e.date << " | " << e.title
                     << " - " << e.description << "\n";
                found = true;
            }
        if (!found) cout << "No upcoming events.\n";
    }

    vector<CalendarEvent>& getEvents() { return events; }
    void setNextId(int id) { nextId = id; }
};
