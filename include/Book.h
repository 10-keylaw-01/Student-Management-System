#pragma once
#include <string>
#include <vector>
#include <iostream>

/// Book structure with issue date and return date tracking
struct Book {
    int id;
    std::string title, author;
    bool available;
    int issuedTo;  // studentId, -1 if available
    std::string issueDate, dueDate, returnDate;
};

/// Manages library books
class Library {
    std::vector<Book> books;
    int nextId = 1;
public:
    void setNextId(int id) { nextId = id; }
    
    /// Add a new book to the library
    void addBook(const std::string& title, const std::string& author) {
        books.push_back({nextId++, title, author, true, -1, "", "", ""});
        std::cout << "[Book added: \"" << title << "\" by " << author 
             << " (ID: " << (nextId-1) << ")]\n";
    }

    /// Issue a book to a student with due date tracking
    void issueBook(int bookId, int studentId, const std::string& issueDate, 
                   const std::string& dueDate) {
        for (auto& b : books) {
            if (b.id == bookId) {
                if (!b.available) { 
                    std::cout << "Book not available.\n"; 
                    return; 
                }
                b.available = false; 
                b.issuedTo = studentId; 
                b.issueDate = issueDate;
                b.dueDate = dueDate;
                b.returnDate = "";  // Clear return date on issue
                std::cout << "[Book #" << bookId << " issued to student " << studentId 
                     << " | Due: " << dueDate << "]\n";
                return;
            }
        }
        std::cout << "Book not found.\n";
    }

    /// Return a book and record return date
    void returnBook(int bookId, const std::string& returnDate = "") {
        for (auto& b : books) {
            if (b.id == bookId) {
                b.available = true; 
                b.issuedTo = -1; 
                b.returnDate = returnDate;
                std::cout << "[Book #" << bookId << " returned" 
                     << (returnDate.empty() ? "" : " on " + returnDate) << ".]\n";
                return;
            }
        }
        std::cout << "Book not found.\n";
    }

    /// List all books in library with status
    void listBooks() const {
        if (books.empty()) { std::cout << "No books in library.\n"; return; }
        for (const auto& b : books) {
            std::cout << "#" << b.id << " | " << b.title << " by " << b.author
                 << " | " << (b.available ? "Available" : "Issued");
            if (!b.available) {
                std::cout << " to ID:" << b.issuedTo << " | Due:" << b.dueDate;
            }
            std::cout << "\n";
        }
    }

    std::vector<Book>& getBooks() { return books; }
};
