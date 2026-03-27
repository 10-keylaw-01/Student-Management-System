#pragma once
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <iomanip>
#include <sstream>
using namespace std;

struct FeeRecord {
    int invoiceId, studentId;
    string studentName, className, dueDate, paidDate;
    double amount, paid;
    bool settled;
};

struct FeeStructure {
    string className;
    double termlyFee;
};

class FeeManager {
    vector<FeeRecord> records;
    vector<FeeStructure> structures;
    int nextInvId = 1;

    // Returns days between two YYYY-MM-DD dates (positive if due < today)
    static int daysPastDue(const string& dueDate, const string& today) {
        auto parse = [](const string& d) {
            int y = stoi(d.substr(0,4)), m = stoi(d.substr(5,2)), day = stoi(d.substr(8,2));
            // Accurate day count accounting for leap years
            int days = y * 365 + day;
            int mdays[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
            // Add leap days for all years before current year
            for (int i = 1; i < y; i++) {
                if ((i%4==0 && i%100!=0) || i%400==0) days++;
            }
            // Add days for months in current year
            for (int i = 1; i < m; i++) {
                days += mdays[i];
                // Add leap day if February and leap year
                if (i == 2 && ((y%4==0 && y%100!=0) || y%400==0)) days++;
            }
            return days;
        };
        return parse(today) - parse(dueDate);
    }

public:
    static const double LATE_FEE_PER_DAY;

    // ── Fee Structure ──────────────────────────────────────────────────────────
    void setFeeStructure(const string& cls, double fee) {
        for (auto& s : structures) { if (s.className == cls) { s.termlyFee = fee; return; } }
        structures.push_back({cls, fee});
    }

    double getFeeForClass(const string& cls) const {
        for (auto& s : structures) if (s.className == cls) return s.termlyFee;
        return 0.0;
    }

    void listFeeStructure() const {
        if (structures.empty()) { cout << "No fee structure defined.\n"; return; }
        cout << "\n--- Fee Structure ---\n";
        for (auto& s : structures)
            cout << "Class " << s.className << ": " << fixed << setprecision(2) << s.termlyFee << "\n";
    }

    vector<FeeStructure>& getStructures() { return structures; }

    // ── Invoices ───────────────────────────────────────────────────────────────
    void generateInvoice(int studentId, const string& name, const string& cls,
                         double amount, const string& dueDate) {
        records.push_back({nextInvId++, studentId, name, cls, dueDate, "", amount, 0, false});
        cout << "[Invoice #" << (nextInvId-1) << " generated for " << name
             << " (Class " << cls << ") - Amount: " << fixed << setprecision(2) << amount << "]\n";
    }

    // Auto-generate invoice from fee structure
    void generateFromStructure(int studentId, const string& name,
                                const string& cls, const string& dueDate) {
        double fee = getFeeForClass(cls);
        if (fee <= 0) { cout << "No fee structure set for class " << cls << ".\n"; return; }
        generateInvoice(studentId, name, cls, fee, dueDate);
    }

    void recordPayment(int invoiceId, double amount, const string& date) {
        for (auto& r : records) {
            if (r.invoiceId == invoiceId) {
                r.paid += amount;
                r.paidDate = date;
                if (r.paid >= r.amount) r.settled = true;
                cout << "[Payment of " << fixed << setprecision(2) << amount
                     << " recorded for Invoice #" << invoiceId << "]\n";
                printReceipt(r);
                return;
            }
        }
        cout << "Invoice not found.\n";
    }

    void checkOutstanding(int studentId, const string& today = "") const {
        bool found = false;
        for (auto& r : records) {
            if (r.studentId == studentId) {
                found = true;
                double late = 0;
                if (!r.settled && !today.empty() && !r.dueDate.empty())
                    late = max(0, daysPastDue(r.dueDate, today)) * LATE_FEE_PER_DAY;
                cout << "Invoice #" << r.invoiceId
                     << " | Amount: " << fixed << setprecision(2) << r.amount
                     << " | Paid: " << r.paid
                     << " | Due: " << r.dueDate;
                if (late > 0) cout << " | Late Fee: " << late;
                cout << " | Status: " << (r.settled ? "Settled" : "Outstanding") << "\n";
            }
        }
        if (!found) cout << "No fee records found.\n";
    }

    void listAll() const {
        if (records.empty()) { cout << "No fee records.\n"; return; }
        cout << "\n--- All Fee Records ---\n";
        for (auto& r : records)
            cout << "Invoice #" << r.invoiceId << " | " << r.studentName
                 << " | Class: " << r.className
                 << " | " << fixed << setprecision(2) << r.amount
                 << " | " << (r.settled ? "Settled" : "Outstanding") << "\n";
    }

    void printReceipt(const FeeRecord& r) const {
        cout << "\n========== RECEIPT ==========\n"
             << "Receipt for Invoice #" << r.invoiceId << "\n"
             << "Student : " << r.studentName << "\n"
             << "Class   : " << r.className << "\n"
             << "Amount  : " << fixed << setprecision(2) << r.amount << "\n"
             << "Paid    : " << r.paid << "\n"
             << "Date    : " << r.paidDate << "\n"
             << "Status  : " << (r.settled ? "SETTLED" : "PARTIAL") << "\n"
             << "=============================\n";
    }

    void printReceiptById(int invoiceId) const {
        for (auto& r : records) if (r.invoiceId == invoiceId) { printReceipt(r); return; }
        cout << "Invoice not found.\n";
    }

    vector<FeeRecord>& getRecords() { return records; }
    void setNextId(int id) { nextInvId = id; }
};

inline const double FeeManager::LATE_FEE_PER_DAY = 1.0;
