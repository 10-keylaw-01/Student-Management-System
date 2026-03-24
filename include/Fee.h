#pragma once
#include <string>
#include <vector>
#include <iostream>
using namespace std;

struct FeeRecord {
    int invoiceId, studentId;
    string studentName, dueDate, paidDate;
    double amount, paid;
    bool settled;
};

class FeeManager {
    vector<FeeRecord> records;
    int nextId = 1;
public:
    void generateInvoice(int studentId, const string& name, double amount, const string& dueDate) {
        records.push_back({nextId++, studentId, name, dueDate, "", amount, 0, false});
        cout << "[Invoice #" << (nextId-1) << " generated for " << name << " - Amount: " << amount << "]\n";
    }

    void recordPayment(int invoiceId, double amount, const string& date) {
        for (auto& r : records) {
            if (r.invoiceId == invoiceId) {
                r.paid += amount;
                r.paidDate = date;
                if (r.paid >= r.amount) r.settled = true;
                cout << "[Payment of " << amount << " recorded for Invoice #" << invoiceId << "]\n";
                return;
            }
        }
        cout << "Invoice not found.\n";
    }

    void checkOutstanding(int studentId) const {
        bool found = false;
        for (auto& r : records) {
            if (r.studentId == studentId) {
                found = true;
                cout << "Invoice #" << r.invoiceId << " | Amount: " << r.amount
                     << " | Paid: " << r.paid << " | Due: " << r.dueDate
                     << " | Status: " << (r.settled ? "Settled" : "Outstanding") << "\n";
            }
        }
        if (!found) cout << "No fee records found.\n";
    }

    void listAll() const {
        if (records.empty()) { cout << "No fee records.\n"; return; }
        for (auto& r : records)
            cout << "Invoice #" << r.invoiceId << " | " << r.studentName
                 << " | " << r.amount << " | " << (r.settled ? "Settled" : "Outstanding") << "\n";
    }

    vector<FeeRecord>& getRecords() { return records; }
};
