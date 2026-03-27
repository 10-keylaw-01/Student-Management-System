# School Management System (SMS)

A comprehensive student management system with both console-based C++ backend and modern PyQt5 GUI frontend.
Manages students, teachers, parents, fees, attendance, grades, timetables, announcements, assignments, 
exams, courses, notifications, and an academic calendar. All data is persisted to CSV files so it survives between runs.

---

## 🎨 Quick Start

### Option 1: PyQt5 GUI Frontend (Recommended)

#### Prerequisites
- Python 3.8+
- PyQt5

#### Setup
```bash
# Install dependencies
pip install -r frontend/requirements.txt

# Run the application
python frontend/main.py
```

The GUI will automatically detect and launch the C++ backend. Features include:
- ✅ Modern, professional interface with role-based dashboards
- ✅ Auto-detecting login with account lockout display
- ✅ Data visualization with tables and statistics
- ✅ Real-time backend communication via subprocess
- ✅ Responsive threading for non-blocking operations

### Option 2: Console Application

#### Build & Run
```bash
g++ -std=c++17 -o sms main.cpp
./sms
```

---

## How to Build & Run

### Backend (C++)

```bash
g++ -std=c++17 -o sms main.cpp
./sms
```

### Frontend (PyQt5)

```bash
cd frontend
pip install -r requirements.txt
python main.py
```

---

## Login & Security

### What happens when you log in
When you successfully log in, the system greets you with a personalised banner:

```
╔══════════════════════════════════════════╗
║                                          ║
║   Hey, John Mensah                       ║
║   You are logged in as: Student          ║
║                                          ║
╚══════════════════════════════════════════╝
  Notifications:
  [Attendance] 2024-01-15 - Absence recorded on 2024-01-15
```

Unread notifications are shown immediately after login and marked as read.

### Failed login attempts & lockout
The system tracks failed login attempts **per username**:

| Attempt | What you see |
|---------|-------------|
| 1st failure | `[!] Invalid credentials. 2 attempt(s) remaining.` |
| 2nd failure | `[!] Invalid credentials. 1 attempt(s) remaining.` |
| 3rd failure | `[!] Account locked for 15 minutes due to too many failed attempts.` |
| Any attempt while locked | `[!] Account is locked. Please wait 15 minutes.` |

After **15 minutes** the lockout clears automatically and you can try again.
The lockout is **per username** — other accounts are not affected.
Empty or whitespace-only usernames/passwords are rejected before any attempt is counted.

---

## Default Login Credentials

### Admin

| Username | Password  |
|----------|-----------|
| `admin`  | `admin123`|

---

### Teachers

| Username   | Password   | Name              | Subject     | Department  |
|------------|------------|-------------------|-------------|-------------|
| `t.ali`    | `teach001` | Mr. Ali Hassan    | Mathematics | Science     |
| `t.sara`   | `teach002` | Ms. Sara Khan     | English     | Languages   |
| `t.james`  | `teach003` | Mr. James Osei    | Physics     | Science     |
| `t.fatima` | `teach004` | Ms. Fatima Noor   | Biology     | Science     |
| `t.david`  | `teach005` | Mr. David Mensah  | History     | Humanities  |

---

### Students

#### Class 10A

| Username      | Password | Name              | Roll |
|---------------|----------|-------------------|------|
| `s.john`      | `stu001` | John Mensah       | R001 |
| `s.amina`     | `stu002` | Amina Boateng     | R002 |
| `s.kwame`     | `stu003` | Kwame Asante      | R003 |
| `s.linda`     | `stu004` | Linda Ofori       | R004 |
| `s.eric`      | `stu005` | Eric Tetteh       | R005 |
| `s.grace`     | `stu006` | Grace Adjei       | R006 |
| `s.samuel`    | `stu007` | Samuel Darko      | R007 |
| `s.abena`     | `stu008` | Abena Owusu       | R008 |
| `s.kofi`      | `stu009` | Kofi Agyeman      | R009 |
| `s.diana`     | `stu010` | Diana Appiah      | R010 |
| `s.felix`     | `stu011` | Felix Osei        | R011 |
| `s.nana`      | `stu012` | Nana Ama Asare    | R012 |
| `s.yaw`       | `stu013` | Yaw Boateng       | R013 |
| `s.akua`      | `stu014` | Akua Frimpong     | R014 |

#### Class 10B

| Username      | Password | Name              | Roll |
|---------------|----------|-------------------|------|
| `s.ben`       | `stu015` | Benjamin Quaye    | R015 |
| `s.esi`       | `stu016` | Esi Mensah        | R016 |
| `s.kojo`      | `stu017` | Kojo Antwi        | R017 |
| `s.adwoa`     | `stu018` | Adwoa Sarpong     | R018 |
| `s.nii`       | `stu019` | Nii Lante Tetteh  | R019 |
| `s.efua`      | `stu020` | Efua Darko        | R020 |
| `s.kwesi`     | `stu021` | Kwesi Asante      | R021 |
| `s.abena2`    | `stu022` | Abena Ofori       | R022 |
| `s.fiifi`     | `stu023` | Fiifi Agyeman     | R023 |
| `s.akosua`    | `stu024` | Akosua Appiah     | R024 |
| `s.ato`       | `stu025` | Ato Mensah        | R025 |
| `s.maame`     | `stu026` | Maame Serwaa      | R026 |
| `s.kobbina`   | `stu027` | Kobbina Asare     | R027 |
| `s.afia`      | `stu028` | Afia Boateng      | R028 |

#### Class 10C

| Username      | Password | Name                | Roll |
|---------------|----------|---------------------|------|
| `s.kwabena`   | `stu029` | Kwabena Frimpong    | R029 |
| `s.ama`       | `stu030` | Ama Quaye           | R030 |
| `s.kwame2`    | `stu031` | Kwamedua Osei       | R031 |
| `s.adjoa`     | `stu032` | Adjoa Antwi         | R032 |
| `s.nana2`     | `stu033` | Nana Yaw Sarpong    | R033 |
| `s.akua2`     | `stu034` | Akua Tetteh         | R034 |
| `s.kweku`     | `stu035` | Kweku Darko         | R035 |
| `s.aba`       | `stu036` | Aba Asante          | R036 |
| `s.yoofi`     | `stu037` | Yoofi Mensah        | R037 |
| `s.adzo`      | `stu038` | Adzo Ofori          | R038 |
| `s.kwabena2`  | `stu039` | Kwabenah Agyeman    | R039 |
| `s.serwaa`    | `stu040` | Serwaa Appiah       | R040 |
| `s.kofi2`     | `stu041` | Kofi Asare          | R041 |
| `s.abena3`    | `stu042` | Abena Boateng       | R042 |

#### Class 11A

| Username      | Password | Name              | Roll |
|---------------|----------|-------------------|------|
| `s.james`     | `stu043` | James Frimpong    | R043 |
| `s.mary`      | `stu044` | Mary Quaye        | R044 |
| `s.peter`     | `stu045` | Peter Osei        | R045 |
| `s.sarah`     | `stu046` | Sarah Antwi       | R046 |
| `s.david2`    | `stu047` | David Sarpong     | R047 |
| `s.ruth`      | `stu048` | Ruth Tetteh       | R048 |
| `s.paul`      | `stu049` | Paul Darko        | R049 |
| `s.esther`    | `stu050` | Esther Asante     | R050 |
| `s.mark`      | `stu051` | Mark Mensah       | R051 |
| `s.lydia`     | `stu052` | Lydia Ofori       | R052 |
| `s.stephen`   | `stu053` | Stephen Agyeman   | R053 |
| `s.hannah`    | `stu054` | Hannah Appiah     | R054 |
| `s.joseph`    | `stu055` | Joseph Asare      | R055 |
| `s.naomi`     | `stu056` | Naomi Boateng     | R056 |

#### Class 11B

| Username      | Password | Name              | Roll |
|---------------|----------|-------------------|------|
| `s.daniel`    | `stu057` | Daniel Frimpong   | R057 |
| `s.rebecca`   | `stu058` | Rebecca Quaye     | R058 |
| `s.andrew`    | `stu059` | Andrew Osei       | R059 |
| `s.deborah`   | `stu060` | Deborah Antwi     | R060 |
| `s.philip`    | `stu061` | Philip Sarpong    | R061 |
| `s.judith`    | `stu062` | Judith Tetteh     | R062 |
| `s.thomas`    | `stu063` | Thomas Darko      | R063 |
| `s.rachel`    | `stu064` | Rachel Asante     | R064 |
| `s.simon`     | `stu065` | Simon Mensah      | R065 |
| `s.leah`      | `stu066` | Leah Ofori        | R066 |
| `s.joshua`    | `stu067` | Joshua Agyeman    | R067 |
| `s.miriam`    | `stu068` | Miriam Appiah     | R068 |
| `s.caleb`     | `stu069` | Caleb Asare       | R069 |
| `s.abigail`   | `stu070` | Abigail Boateng   | R070 |

#### Class 11C

| Username       | Password | Name              | Roll |
|----------------|----------|-------------------|------|
| `s.isaac`      | `stu071` | Isaac Frimpong    | R071 |
| `s.priscilla`  | `stu072` | Priscilla Quaye   | R072 |
| `s.elijah`     | `stu073` | Elijah Osei       | R073 |
| `s.dorcas`     | `stu074` | Dorcas Antwi      | R074 |
| `s.aaron`      | `stu075` | Aaron Sarpong     | R075 |
| `s.eunice`     | `stu076` | Eunice Tetteh     | R076 |
| `s.moses`      | `stu077` | Moses Darko       | R077 |
| `s.phoebe`     | `stu078` | Phoebe Asante     | R078 |
| `s.gideon`     | `stu079` | Gideon Mensah     | R079 |
| `s.tabitha`    | `stu080` | Tabitha Ofori     | R080 |

---

### Parents
Each parent is linked to one student (shown in the Linked Student column).

| Username        | Password | Name                    | Linked Student      |
|-----------------|----------|-------------------------|---------------------|
| `p.mensah`      | `par001` | Mr. Kweku Mensah        | John Mensah         |
| `p.boateng`     | `par002` | Mrs. Ama Boateng        | Amina Boateng       |
| `p.asante`      | `par003` | Mr. Yaw Asante          | Kwame Asante        |
| `p.ofori`       | `par004` | Mrs. Akua Ofori         | Linda Ofori         |
| `p.tetteh`      | `par005` | Mr. Nii Tetteh          | Eric Tetteh         |
| `p.adjei`       | `par006` | Mrs. Efua Adjei         | Grace Adjei         |
| `p.darko`       | `par007` | Mr. Kojo Darko          | Samuel Darko        |
| `p.owusu`       | `par008` | Mrs. Adwoa Owusu        | Abena Owusu         |
| `p.agyeman`     | `par009` | Mr. Fiifi Agyeman       | Kofi Agyeman        |
| `p.appiah`      | `par010` | Mrs. Abena Appiah       | Diana Appiah        |
| `p.osei`        | `par011` | Mr. Kwame Osei          | Felix Osei          |
| `p.asare`       | `par012` | Mrs. Nana Asare         | Nana Ama Asare      |
| `p.boateng2`    | `par013` | Mr. Yaw Boateng         | Yaw Boateng         |
| `p.frimpong`    | `par014` | Mrs. Akua Frimpong      | Akua Frimpong       |
| `p.quaye`       | `par015` | Mr. Benjamin Quaye Sr   | Benjamin Quaye      |
| `p.mensah2`     | `par016` | Mrs. Esi Mensah         | Esi Mensah          |
| `p.antwi`       | `par017` | Mr. Kojo Antwi Sr       | Kojo Antwi          |
| `p.sarpong`     | `par018` | Mrs. Adwoa Sarpong      | Adwoa Sarpong       |
| `p.tetteh2`     | `par019` | Mr. Nii Tetteh Sr       | Nii Lante Tetteh    |
| `p.darko2`      | `par020` | Mrs. Efua Darko         | Efua Darko          |
| `p.asante2`     | `par021` | Mr. Kwesi Asante Sr     | Kwesi Asante        |
| `p.ofori2`      | `par022` | Mrs. Abena Ofori        | Abena Ofori         |
| `p.agyeman2`    | `par023` | Mr. Fiifi Agyeman Sr    | Fiifi Agyeman       |
| `p.appiah2`     | `par024` | Mrs. Akosua Appiah      | Akosua Appiah       |
| `p.mensah3`     | `par025` | Mr. Ato Mensah Sr       | Ato Mensah          |
| `p.serwaa`      | `par026` | Mrs. Maame Serwaa       | Maame Serwaa        |
| `p.asare2`      | `par027` | Mr. Kobbina Asare Sr    | Kobbina Asare       |
| `p.boateng3`    | `par028` | Mrs. Afia Boateng       | Afia Boateng        |
| `p.frimpong2`   | `par029` | Mr. Kwabena Frimpong Sr | Kwabena Frimpong    |
| `p.quaye2`      | `par030` | Mrs. Ama Quaye          | Ama Quaye           |
| `p.osei2`       | `par031` | Mr. Kwame Osei Jr       | Kwamedua Osei       |
| `p.antwi2`      | `par032` | Mrs. Adjoa Antwi        | Adjoa Antwi         |
| `p.sarpong2`    | `par033` | Mr. Nana Sarpong        | Nana Yaw Sarpong    |
| `p.tetteh3`     | `par034` | Mrs. Akua Tetteh        | Akua Tetteh         |
| `p.darko3`      | `par035` | Mr. Kweku Darko Sr      | Kweku Darko         |
| `p.asante3`     | `par036` | Mrs. Aba Asante         | Aba Asante          |
| `p.mensah4`     | `par037` | Mr. Yoofi Mensah Sr     | Yoofi Mensah        |
| `p.ofori3`      | `par038` | Mrs. Adzo Ofori         | Adzo Ofori          |
| `p.agyeman3`    | `par039` | Mr. Kwabenah Agyeman Sr | Kwabenah Agyeman    |
| `p.appiah3`     | `par040` | Mrs. Serwaa Appiah      | Serwaa Appiah       |
| `p.asare3`      | `par041` | Mr. Kofi Asare Sr       | Kofi Asare          |
| `p.boateng4`    | `par042` | Mrs. Abena Boateng      | Abena Boateng       |
| `p.frimpong3`   | `par043` | Mr. James Frimpong Sr   | James Frimpong      |
| `p.quaye3`      | `par044` | Mrs. Mary Quaye         | Mary Quaye          |
| `p.osei3`       | `par045` | Mr. Peter Osei Sr       | Peter Osei          |
| `p.antwi3`      | `par046` | Mrs. Sarah Antwi        | Sarah Antwi         |
| `p.sarpong3`    | `par047` | Mr. David Sarpong Sr    | David Sarpong       |
| `p.tetteh4`     | `par048` | Mrs. Ruth Tetteh        | Ruth Tetteh         |
| `p.darko4`      | `par049` | Mr. Paul Darko Sr       | Paul Darko          |
| `p.asante4`     | `par050` | Mrs. Esther Asante      | Esther Asante       |
| `p.mensah5`     | `par051` | Mr. Mark Mensah Sr      | Mark Mensah         |
| `p.ofori4`      | `par052` | Mrs. Lydia Ofori        | Lydia Ofori         |
| `p.agyeman4`    | `par053` | Mr. Stephen Agyeman Sr  | Stephen Agyeman     |
| `p.appiah4`     | `par054` | Mrs. Hannah Appiah      | Hannah Appiah       |
| `p.asare4`      | `par055` | Mr. Joseph Asare Sr     | Joseph Asare        |
| `p.boateng5`    | `par056` | Mrs. Naomi Boateng      | Naomi Boateng       |
| `p.frimpong4`   | `par057` | Mr. Daniel Frimpong Sr  | Daniel Frimpong     |
| `p.quaye4`      | `par058` | Mrs. Rebecca Quaye      | Rebecca Quaye       |
| `p.osei4`       | `par059` | Mr. Andrew Osei Sr      | Andrew Osei         |
| `p.antwi4`      | `par060` | Mrs. Deborah Antwi      | Deborah Antwi       |
| `p.sarpong4`    | `par061` | Mr. Philip Sarpong Sr   | Philip Sarpong      |
| `p.tetteh5`     | `par062` | Mrs. Judith Tetteh      | Judith Tetteh       |
| `p.darko5`      | `par063` | Mr. Thomas Darko Sr     | Thomas Darko        |
| `p.asante5`     | `par064` | Mrs. Rachel Asante      | Rachel Asante       |
| `p.mensah6`     | `par065` | Mr. Simon Mensah Sr     | Simon Mensah        |
| `p.ofori5`      | `par066` | Mrs. Leah Ofori         | Leah Ofori          |
| `p.agyeman5`    | `par067` | Mr. Joshua Agyeman Sr   | Joshua Agyeman      |
| `p.appiah5`     | `par068` | Mrs. Miriam Appiah      | Miriam Appiah       |
| `p.asare5`      | `par069` | Mr. Caleb Asare Sr      | Caleb Asare         |
| `p.boateng6`    | `par070` | Mrs. Abigail Boateng    | Abigail Boateng     |
| `p.frimpong5`   | `par071` | Mr. Isaac Frimpong Sr   | Isaac Frimpong      |
| `p.quaye5`      | `par072` | Mrs. Priscilla Quaye    | Priscilla Quaye     |
| `p.osei5`       | `par073` | Mr. Elijah Osei Sr      | Elijah Osei         |
| `p.antwi5`      | `par074` | Mrs. Dorcas Antwi       | Dorcas Antwi        |
| `p.sarpong5`    | `par075` | Mr. Aaron Sarpong Sr    | Aaron Sarpong       |
| `p.tetteh6`     | `par076` | Mrs. Eunice Tetteh      | Eunice Tetteh       |
| `p.darko6`      | `par077` | Mr. Moses Darko Sr      | Moses Darko         |
| `p.asante6`     | `par078` | Mrs. Phoebe Asante      | Phoebe Asante       |
| `p.mensah7`     | `par079` | Mr. Gideon Mensah Sr    | Gideon Mensah       |
| `p.ofori6`      | `par080` | Mrs. Tabitha Ofori      | Tabitha Ofori       |

---

## Default Fee Structure

| Class | Termly Fee |
|-------|-----------|
| 10A   | 1,500.00  |
| 10B   | 1,500.00  |
| 10C   | 1,500.00  |
| 11A   | 1,800.00  |
| 11B   | 1,800.00  |
| 11C   | 1,800.00  |
| 12A   | 2,000.00  |
| 12B   | 2,000.00  |

Admin can update these at any time via **Fee Management → Set Fee Structure**.
Late fee: **1.00 per day** after the due date.

---

## Project Structure

```
SMS/
├── main.cpp              # Entry point: login, menus, global state
├── include/
│   ├── User.h            # Base class + Role enum
│   ├── Student.h         # Student class
│   ├── Teacher.h         # Teacher class
│   ├── Admin.h           # Admin + Parent classes
│   ├── Fee.h             # Enhanced fee management (late fees, receipts, structure)
│   ├── Attendance.h      # Attendance tracking
│   ├── Grade.h           # Grade management
│   ├── Timetable.h       # Class schedule
│   ├── Announcement.h    # Communication portal
│   ├── Assignment.h      # LMS: assignments + materials
│   ├── Book.h            # Library management
│   ├── Exam.h            # Exam scheduling + results + grade letters
│   ├── Course.h          # Course management (admin CRUD, student view)
│   ├── Notification.h    # In-app notification log (CSV-backed)
│   ├── Calendar.h        # Academic calendar + events
│   ├── Analytics.h       # Attendance & grade analytics/reports
│   ├── Auth.h            # Authentication with lockout protection
│   ├── Crypto.h          # Password hashing (FNV-1a algorithm)
│   ├── FileManager.h     # CSV persistence layer
│   └── menus/            # User-facing menu implementations
│       ├── StudentMenu.h
│       ├── TeacherMenu.h
│       └── ParentMenu.h
├── data/                 # CSV data files (persistent storage)
│   ├── students.csv
│   ├── teachers.csv
│   ├── admins.csv
│   ├── parents.csv
│   ├── fees.csv
│   ├── attendance.csv
│   ├── grades.csv
│   ├── timetable.csv
│   ├── announcements.csv
│   ├── assignments.csv
│   ├── materials.csv
│   ├── books.csv
│   ├── exams.csv
│   ├── exam_results.csv
│   ├── courses.csv
│   ├── notifications.csv
│   ├── calendar.csv
│   ├── fee_structure.csv
│   └── calibration.csv
└── README.md             # This file
```

---

## What's New in This Version

### Critical Bug Fixes (6 fixes applied)

1. **Missing getValidInt Overload** ✅
   - Added `int getValidInt(const std::string&, int, int)` 3-argument version in main.cpp
   - Calls in adminMenu for student/teacher removal now use explicit range parameters
   - Compiles cleanly without ambiguity errors

2. **Missing extern Declaration** ✅
   - StudentMenu.h now declares `int getValidInt(const std::string& prompt, int minVal, int maxVal)`
   - Eliminates implicit declaration warning

3. **Missing Fee Structures** ✅
   - Added "10C" (1,500.00) and "11C" (1,800.00) to default fee structures in seedDefaults()
   - All three classes per grade level now have proper fee configuration

4. **Broken Leap-Year Loop in daysPastDue** ✅
   - Replaced O(n) year-loop implementation with O(1) `std::mktime`-based algorithm
   - daysPastDue() now calculates days between dates using standard library functions
   - Performance: ~2024 iterations reduced to constant time for any date

5. **Overpayment Warning Typo** ✅
   - Fixed extra `]` bracket in overpayment warning message
   - Now displays: `[WARNING] Overpayment detected: 50.00\n` instead of `[WARNING] Overpayment detected: 50.00]\n`

6. **Teacher Authorization Bypass** ✅
   - TeacherMenu now properly checks if student exists BEFORE authorization check
   - When marking attendance/grades, now:
     1. Searches for student by ID
     2. If `cls` is empty (student not found), prints error and continues to menu
     3. Only proceeds if student is found AND teacher is assigned to that class
   - Applies to: Mark Attendance (Choice 1) and Add Grades (Choice 2)

---

### Security & Robustness Improvements

- **Password Hashing**: All passwords use Crypto::hashPassword() (FNV-1a hashing)
- **Input Validation**: getValidInt() with range checking prevents malformed numeric input
- **Late Fee Calculation**: Uses proper date arithmetic instead of iterating through years
- **Authorization**: Teachers can only manage students in their assigned classes
- **Duplicate Prevention**: Username, attendance records, grades all protected from duplicates

---

### Compilation & Testing

```bash
# Build (no warnings or errors)
g++ -std=c++17 -o sms main.cpp

# Run
./sms
```

All fixes have been verified to compile cleanly and preserve existing functionality.
│   ├── Auth.h            # Login attempt limiting + input sanitization
│   └── FileManager.h     # CSV read/write for all data
└── data/
    ├── students.csv
    ├── teachers.csv
    ├── admins.csv
    ├── parents.csv
    ├── fees.csv
    ├── fee_structure.csv
    ├── attendance.csv
    ├── grades.csv
    ├── timetable.csv
    ├── announcements.csv
    ├── assignments.csv
    ├── materials.csv
    ├── books.csv
    ├── exams.csv
    ├── exam_results.csv
    ├── courses.csv
    ├── notifications.csv
    └── calendar.csv
```

---

## Role Capabilities

| Feature                        | Admin | Teacher | Student | Parent |
|-------------------------------|:-----:|:-------:|:-------:|:------:|
| Manage students/teachers       | ✓     |         |         |        |
| Fee management & receipts      | ✓     |         |         |        |
| View own fee status            |       |         | ✓       | ✓      |
| Mark attendance                |       | ✓       |         |        |
| View attendance                | ✓     | ✓       | ✓       | ✓      |
| Add/view grades                |       | ✓       | ✓       | ✓      |
| Schedule exams                 | ✓     | ✓       |         |        |
| Enter exam results             | ✓     | ✓       |         |        |
| View exam results              |       |         | ✓       | ✓      |
| Course CRUD                    | ✓     |         |         |        |
| View courses                   | ✓     |         | ✓       |        |
| Analytics & reports            | ✓     | ✓       |         |        |
| Academic calendar              | ✓     |         | ✓       | ✓      |
| Send announcements             | ✓     | ✓       |         |        |
| Notifications                  | ✓     | ✓       | ✓       | ✓      |
| Library management             | ✓     |         |         |        |
| Timetable management           | ✓     |         | ✓       |        |

---

## Frontend (PyQt5) Structure

```
frontend/
├── main.py                      # Application entry point
├── backend_communicator.py      # Subprocess handler for C++ backend
├── requirements.txt             # Python dependencies (PyQt5)
├── styles.qss                   # Modern stylesheet
├── ui/                          # UI modules
│   ├── __init__.py
│   ├── login_window.py          # Login interface with lockout tracking
│   ├── admin_dashboard.py       # Admin role dashboard
│   ├── teacher_dashboard.py     # Teacher role dashboard
│   ├── student_dashboard.py     # Student role dashboard
│   └── parent_dashboard.py      # Parent role dashboard
└── utils/                       # Utility modules
    ├── __init__.py
    ├── parser.py                # C++ output parsing utilities
    └── widgets.py               # Custom PyQt5 widgets
```

### Frontend Features

- **Modern GUI**: Clean, professional interface with role-based dashboards
- **Backend Communication**: Non-blocking subprocess communication via threading
- **Data Tables**: QTableWidget for displaying CSV data (students, fees, attendance, etc.)
- **Forms & Dialogs**: QDialog for adding/editing records
- **Login Security**: Failed attempt tracking with 15-minute account lockout
- **Password Toggle**: Show/hide password option in login
- **Responsive UI**: QThread prevents GUI freezing during backend operations
- **Output Parsing**: Auto-detects C++ console output patterns and converts to widgets
- **Role-Based Access**: Different dashboards for Admin, Teacher, Student, and Parent roles
- **Statistics Widgets**: Summary cards for quick metrics (attendance, grades, fees, etc.)

