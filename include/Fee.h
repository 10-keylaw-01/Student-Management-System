#pragma once
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cmath>

/// Fee record structure
struct FeeRecord {
    int invoiceId, studentId;
    std::string studentName, className, dueDate, paidDate;
    double amount, paid;
    bool settled;
};

/// Fee structure for classes
struct FeeStructure {
    std::string className;
    double termlyFee;
};

/// Manages student fees and invoices
class FeeManager {
    std::vector<FeeRecord> records;
    std::vector<FeeStructure> structures;
    int nextInvId = 1;

    // Calculate days past due (positive if overdue)
    static int daysPastDue(const std::string& dueDate, const std::string& today) {
        auto toTm = [](const std::string& d) -> std::time_t {
            std::tm t{};
            t.tm_year = std::stoi(d.substr(0,4)) - 1900;
            t.tm_mon  = std::stoi(d.substr(5,2)) - 1;
            t.tm_mday = std::stoi(d.substr(8,2));
            t.tm_isdst = -1;
            return std::mktime(&t);
        };
        double diff = std::difftime(toTm(today), toTm(dueDate));
        return static_cast<int>(diff / 86400.0);
    }

public:
    static const double LATE_FEE_PER_DAY;

    /// Set or update fee structure for a class
    void setFeeStructure(const std::string& cls, double fee) {
        for (auto& s : structures) { 
            if (s.className == cls) { 
                s.termlyFee = fee; 
                return; 
            } 
        }
        structures.push_back({cls, fee});
    }

    /// Get fee amount for a specific class
    double getFeeForClass(const std::string& cls) const {
        for (const auto& s : structures) if (s.className == cls) return s.termlyFee;
        return 0.0;
    }

    /// List all fee structures
    void listFeeStructure() const {
        if (structures.empty()) { std::cout << "No fee structure defined.\n"; return; }
        std::cout << "\n--- Fee Structure ---\n";
        for (const auto& s : structures)
            std::cout << "Class " << s.className << ": " << std::fixed << std::setprecision(2) 
                 << s.termlyFee << "\n";
    }

    std::vector<FeeStructure>& getStructures() { return structures; }

    /// Generate invoice for a student
    void generateInvoice(int studentId, const std::string& name, const std::string& cls,
                         double amount, const std::string& dueDate) {
        records.push_back({nextInvId++, studentId, name, cls, dueDate, "", amount, 0, false});
        std::cout << "[Invoice #" << (nextInvId-1) << " generated for " << name
             << " (Class " << cls << ") - Amount: " << std::fixed << std::setprecision(2) 
             << amount << "]\n";
    }

    /// Auto-generate invoice from fee structure
    void generateFromStructure(int studentId, const std::string& name,
                                const std::string& cls, const std::string& dueDate) {
        double fee = getFeeForClass(cls);
        if (fee <= 0) { 
            std::cout << "No fee structure set for class " << cls << ".\n"; 
            return; 
        }
        generateInvoice(studentId, name, cls, fee, dueDate);
    }

    /// Record payment for an invoice (with overpayment warning)
    void recordPayment(int invoiceId, double amount, const std::string& date) {
        for (auto& r : records) {
            if (r.invoiceId == invoiceId) {
                r.paid += amount;
                r.paidDate = date;
                
                // Check for overpayment
                if (r.paid > r.amount) {
                    double overpaid = r.paid - r.amount;
                    std::cout << "[WARNING] Overpayment detected: " << std::fixed 
                         << std::setprecision(2) << overpaid << "\n";
                    r.paid = r.amount;  // Cap at actual amount due
                }
                
                if (r.paid >= r.amount) r.settled = true;
                std::cout << "[Payment of " << std::fixed << std::setprecision(2) << amount
                     << " recorded for Invoice #" << invoiceId << "]\n";
                printReceipt(r);
                return;
            }
        }
        std::cout << "Invoice not found.\n";
    }

    /// Check outstanding fees for a student
    void checkOutstanding(int studentId, const std::string& today = "") const {
        bool found = false;
        for (const auto& r : records) {
            if (r.studentId == studentId) {
                found = true;
                double late = 0;
                if (!r.settled && !today.empty() && !r.dueDate.empty())
                    late = std::max(0.0, (double)daysPastDue(r.dueDate, today)) * LATE_FEE_PER_DAY;
                std::cout << "Invoice #" << r.invoiceId
                     << " | Amount: " << std::fixed << std::setprecision(2) << r.amount
                     << " | Paid: " << r.paid
                     << " | Due: " << r.dueDate;
                if (late > 0) std::cout << " | Late Fee: " << late;
                std::cout << " | Status: " << (r.settled ? "Settled" : "Outstanding") << "\n";
            }
        }
        if (!found) std::cout << "No fee records found.\n";
    }

    /// List all fee records
    void listAll() const {
        if (records.empty()) { std::cout << "No fee records.\n"; return; }
        std::cout << "\n--- All Fee Records ---\n";
        for (const auto& r : records)
            std::cout << "Invoice #" << r.invoiceId << " | " << r.studentName
                 << " | Class: " << r.className
                 << " | " << std::fixed << std::setprecision(2) << r.amount
                 << " | " << (r.settled ? "Settled" : "Outstanding") << "\n";
    }

    /// Print receipt for a fee record
    void printReceipt(const FeeRecord& r) const {
        std::cout << "\n========== RECEIPT ==========\n"
             << "Receipt for Invoice #" << r.invoiceId << "\n"
             << "Student : " << r.studentName << "\n"
             << "Class   : " << r.className << "\n"
             << "Amount  : " << std::fixed << std::setprecision(2) << r.amount << "\n"
             << "Paid    : " << r.paid << "\n"
             << "Date    : " << r.paidDate << "\n"
             << "Status  : " << (r.settled ? "SETTLED" : "PARTIAL") << "\n"
             << "=============================\n";
    }

    /// Print receipt by invoice ID
    void printReceiptById(int invoiceId) const {
        for (const auto& r : records) 
            if (r.invoiceId == invoiceId) { 
                printReceipt(r); 
                return; 
            }
        std::cout << "Invoice not found.\n";
    }

    std::vector<FeeRecord>& getRecords() { return records; }
    void setNextId(int id) { nextInvId = id; }
};

inline const double FeeManager::LATE_FEE_PER_DAY = 1.0;
