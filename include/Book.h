#pragma once
#include <string>
#include <vector>
#include <iostream>
using namespace std;

struct Book {
    int id;
    string title, author;
    bool available;
    int issuedTo;  // studentId, -1 if available
    string issueDate;
};

class Library {
    vector<Book> books;
    int nextId = 1;
public:
    void setNextId(int id) { nextId = id; }
    void addBook(const string& title, const string& author) {
        books.push_back({nextId++, title, author, true, -1, ""});
        cout << "[Book added: \"" << title << "\" by " << author << " (ID: " << (nextId-1) << ")]\n";
    }

    void issueBook(int bookId, int studentId, const string& date) {
        for (auto& b : books) {
            if (b.id == bookId) {
                if (!b.available) { cout << "Book not available.\n"; return; }
                b.available = false; b.issuedTo = studentId; b.issueDate = date;
                cout << "[Book #" << bookId << " issued to student " << studentId << "]\n";
                return;
            }
        }
        cout << "Book not found.\n";
    }

    void returnBook(int bookId) {
        for (auto& b : books) {
            if (b.id == bookId) {
                b.available = true; b.issuedTo = -1; b.issueDate = "";
                cout << "[Book #" << bookId << " returned.]\n";
                return;
            }
        }
        cout << "Book not found.\n";
    }

    void listBooks() const {
        if (books.empty()) { cout << "No books in library.\n"; return; }
        for (auto& b : books)
            cout << "#" << b.id << " | " << b.title << " by " << b.author
                 << " | " << (b.available ? "Available" : "Issued") << "\n";
    }

    vector<Book>& getBooks() { return books; }
};
