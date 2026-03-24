# School Management System (SMS)

A console-based C++ application that manages students, teachers, parents, fees, attendance, grades, timetables, announcements, assignments, and a library. All data is persisted to CSV files so it survives between runs.

---

## How to Build & Run

```bash
g++ -std=c++17 -o sms main.cpp
./sms
```

---

## Default Login Credentials

| Role    | Username      | Password  |
|---------|---------------|-----------|
| Admin   | `admin`       | `admin123`|
| Teacher | `t.ali`       | `teach001` |
| Teacher | `t.sara`      | `teach002` |
| Teacher | `t.james`     | `teach003` |
| Teacher | `t.fatima`    | `teach004` |
| Teacher | `t.david`     | `teach005` |
| Student | `s.john`      | `stu001`  |
| Student | `s.amina`     | `stu002`  |
| Student | `s.kwame`     | `stu003`  |
| Student | `s.linda`     | `stu004`  |
| Student | `s.eric`      | `stu005`  |
| Student | `s.grace`     | `stu006`  |
| Student | `s.samuel`    | `stu007`  |
| Student | `s.abena`     | `stu008`  |
| Student | `s.kofi`      | `stu009`  |
| Student | `s.diana`     | `stu010`  |
| Parent  | `p.mensah`    | `par001`  |
| Parent  | `p.boateng`   | `par002`  |
| Parent  | `p.asante`    | `par003`  |
| Parent  | `p.ofori`     | `par004`  |
| Parent  | `p.tetteh`    | `par005`  |
| Parent  | `p.adjei`     | `par006`  |
| Parent  | `p.darko`     | `par007`  |
| Parent  | `p.owusu`     | `par008`  |
| Parent  | `p.agyeman`   | `par009`  |
| Parent  | `p.appiah`    | `par010`  |

---

## Pre-loaded Test Data

The CSV files ship with the following seed data so you can test immediately:

- **10 Students** across 3 classes: `10A`, `10B`, `11A`
- **5 Teachers** covering Mathematics, English, Physics, Biology, History
- **10 Parents**, each linked to one student
- **10 Fee invoices** — some settled, some outstanding
- **50 Attendance records** — 5 days of attendance for every student
- **60 Grade entries** — 4–6 subjects per student across Term1 and Term2
- **35 Timetable slots** — full weekly schedule for all 3 classes
- **5 Announcements** — school-wide notices
- **10 Assignments** — spread across all classes and subjects
- **10 Study Materials** — notes and resources per subject/class
- **15 Library Books** — 4 currently issued to students

---

## Project Structure

```
SMS/
├── main.cpp          # Entry point: login, menus, global state
├── User.h            # Base class + Role enum
├── Student.h         # Student class
├── Teacher.h         # Teacher class
├── Admin.h           # Admin + Parent classes
├── Fee.h             # Fee management
├── Attendance.h      # Attendance tracking
├── Grade.h           # Grade management
├── Timetable.h       # Class schedule
├── Announcement.h    # Communication portal
├── Assignment.h      # LMS: assignments + materials
├── Book.h            # Library management
├── FileManager.h     # CSV read/write for all data
└── *.csv             # Persistent data files
```

---

## Code Explanation

### `User.h` — Base Class & Role System

```cpp
enum class Role { Admin, Teacher, Parent, Student };

class User {
public:
    int id;
    string username, password, name;
    Role role;
    virtual void viewDashboard() const = 0;
};
```

`User` is an **abstract base class**. Every account in the system is a `User`. The `Role` enum enforces Role-Based Access Control (RBAC) — each role sees only its own dashboard and menu options. `viewDashboard()` is a **pure virtual function**, meaning every derived class *must* implement it. The `id` field uniquely identifies every user across all roles.

---

### `Student.h` — Student Class

```cpp
class Student : public User {
public:
    string rollNumber, className;
    int parentId;
    vector<AttendanceRecord> attendance;
    vector<GradeRecord> grades;
};
```

Inherits from `User` with `Role::Student`. Stores the student's roll number, class (e.g. `10A`), and a `parentId` linking them to their parent account. The `attendance` and `grades` vectors are in-memory caches — the authoritative data lives in `AttendanceManager` and `GradeManager`. `attendancePercent()` calculates the percentage of days present.

---

### `Teacher.h` — Teacher Class

```cpp
class Teacher : public User {
public:
    string subject, department;
    vector<string> assignedClasses;
};
```

Inherits from `User` with `Role::Teacher`. A teacher has a primary `subject` and `department`. `assignedClasses` tracks which classes they teach. Teachers can mark attendance, add grades, upload assignments/materials, and post announcements.

---

### `Admin.h` — Admin & Parent Classes

**Admin** inherits `User` with `Role::Admin`. It has no extra fields — the admin's power comes from the menu options available to them (manage all users, fees, library, timetable, reports).

**Parent** inherits `User` with `Role::Parent` and adds a `studentId` field that links directly to their child's student record. Parents have a read-only view of their child's attendance, grades, and fee status.

---

### `Fee.h` — Fee Management

```cpp
struct FeeRecord {
    int invoiceId, studentId;
    string studentName, dueDate, paidDate;
    double amount, paid;
    bool settled;
};
```

`FeeManager` holds a `vector<FeeRecord>`. Key methods:
- `generateInvoice()` — creates a new fee record for a student with an amount and due date.
- `recordPayment()` — adds a payment to an invoice; automatically marks it `settled` when `paid >= amount`.
- `checkOutstanding()` — filters records by `studentId` and prints unpaid balances.
- `listAll()` — prints every invoice (used by admin reports).

---

### `Attendance.h` — Attendance Tracking

```cpp
struct AttendanceEntry {
    int studentId;
    string studentName, className, date;
    bool present;
};
```

`AttendanceManager` stores every individual attendance event as a flat list. This makes it easy to query by student, by class, or by date range. `viewByStudent()` also calculates and prints the attendance percentage. `viewByClass()` accepts an optional date filter — pass an empty string to see all dates.

---

### `Grade.h` — Grade Management

```cpp
struct GradeEntry {
    int studentId;
    string studentName, subject, term;
    double marks, total;
};
```

`GradeManager` stores subject-wise grades per student per term. `viewReport()` prints each subject with its percentage, then calculates an **overall percentage** and **GPA** (scaled 0–4.0) across all entries for that student.

---

### `Timetable.h` — Class Schedule

```cpp
struct TimeSlot {
    string className, day, startTime, endTime, subject, teacherName;
};
```

`Timetable` stores time slots as a flat list. `addSlot()` adds a new period. `viewByClass()` filters and prints all slots for a given class name (e.g. `10A`), showing day, time range, subject, and teacher.

---

### `Announcement.h` — Communication Portal

```cpp
struct Announcement {
    int id;
    string title, content, date, author;
    Role targetRole;
    bool allRoles;
};
```

`AnnouncementManager::post()` creates an announcement and simulates a bulk notification by printing a console message. The `allRoles` flag broadcasts to everyone; otherwise it targets a specific role. `view(role)` filters announcements visible to the given role.

---

### `Assignment.h` — Learning Management System (LMS)

Two structs managed by `LMSManager`:

- **`Assignment`** — has a title, subject, due date, file reference, and target class. Teachers upload; students view by their class name.
- **`Material`** — has a title, subject, content path, and target class. Same upload/view pattern as assignments.

Both are filtered by `className` so students only see content relevant to their class.

---

### `Book.h` — Library Management

```cpp
struct Book {
    int id;
    string title, author;
    bool available;
    int issuedTo;   // -1 if available
    string issueDate;
};
```

`Library` manages a collection of books. `issueBook()` checks availability before issuing and records which student has it. `returnBook()` resets `available = true` and clears `issuedTo`. `listBooks()` shows availability status for every book.

---

### `FileManager.h` — CSV Persistence

`FileManager` is a utility class with only `static` methods — no instance needed. It has a `saveX()` and `loadX()` pair for every data type.

- `writeCSV()` — opens a file and writes one record per line, fields separated by commas.
- `readCSV()` — reads a file line by line, splits each line on `,` into a `vector<string>` row, and returns all rows.

Each `loadX()` method validates that a row has enough columns before constructing an object, preventing crashes on malformed data. `saveAll()` and `loadAll()` in `main.cpp` call every save/load method at once.

---

### `main.cpp` — Entry Point & Menus

**Global state** — all data collections (`vector<Student>`, `vector<Teacher>`, etc.) and manager objects live as global variables so every menu function can access them without passing parameters.

**`loadAll()` / `saveAll()`** — called at startup and after every menu action to keep CSV files in sync.

**`nextId()`** — generates a new unique ID by summing the sizes of all user vectors + 1. Simple but effective for a single-session system.

**`login()`** — reads username and password, then searches each user vector in order (Admin → Teacher → Student → Parent). On match, it calls the appropriate role menu.

**Role menus** (`adminMenu`, `teacherMenu`, `studentMenu`, `parentMenu`) — each runs a `do-while` loop showing the dashboard until the user selects Logout. Every action calls `saveAll()` immediately after so no data is lost.

**`seedDefaults()`** — inserts the default admin account only if `admins.csv` is empty or missing, preventing duplicate admins on repeated runs.

---

## Data Flow

```
Startup
  └── loadAll() reads all *.csv files into memory

User logs in
  └── Role-specific menu loop

User performs action (e.g. add grade)
  └── Data added to in-memory vector
  └── saveAll() writes updated vector back to CSV

User logs out → back to main menu

Exit
  └── saveAll() final write
```

---

## Extending the System

| Want to add...         | Where to change                          |
|------------------------|------------------------------------------|
| New user field         | Update the class `.h` file + `FileManager.h` save/load |
| New menu option        | Add a new `else if` branch in the role menu in `main.cpp` |
| New data type          | Create a new `.h` file, add save/load to `FileManager.h`, call from `saveAll()`/`loadAll()` |
| Password hashing       | Replace plain-text password comparison in `login()` |
