# Student Management System

A comprehensive C++ console application for managing school operations including students, teachers, parents, fees, attendance, grades, and more. A PyQt5 GUI frontend is also included.

## Features

### User Roles
- **Admin**: Full system management capabilities
- **Teacher**: Manage classes, grades, attendance, assignments
- **Student**: View personal academic information
- **Parent**: Monitor children's academic progress

### Core Modules
- **User Management**: Students, Teachers, Parents, Admins
- **Academic Management**: Grades, Attendance, Courses, Timetables
- **Fee Management**: Fee structures, invoices, payments
- **Library Management**: Books, issue/return system
- **Exam Management**: Scheduling, results
- **Communication**: Announcements, notifications
- **Learning Management**: Assignments, materials
- **Calendar**: Academic events and schedules

## Project Structure

```
Student-Management-System/
├── main.cpp                 # Main application entry point
├── build.bat               # Build script for Windows
├── seed.py                 # Database seeding script
├── include/                # C++ header files
│   ├── menus/
│   │   ├── StudentMenu.h
│   │   ├── TeacherMenu.h
│   │   └── ParentMenu.h
│   ├── Admin.h, Student.h, Teacher.h, Parent.h
│   ├── Auth.h, Crypto.h, FileManager.h
│   └── ... (other modules)
├── frontend/               # PyQt5 GUI frontend
│   ├── main.py             # Frontend entry point
│   ├── backend_communicator.py
│   ├── requirements.txt
│   ├── ui/                 # Dashboard windows
│   └── utils/              # Widgets and helpers
└── data/                   # CSV data files (created at runtime)
```

## Building and Running

### Prerequisites

- **C++ compiler**: MinGW-w64 (g++) recommended for Windows
- **Python 3.8+** (only required for the PyQt5 frontend)

### Build the C++ Backend

```cmd
build.bat
```

Or manually:

```cmd
g++ -std=c++17 -I. -Wall -Wextra -static-libgcc -static-libstdc++ main.cpp -o sms.exe
```

> The `-static-libgcc -static-libstdc++` flags bundle the MinGW runtime into the executable so it runs without extra DLLs. If you build **without** those flags you must keep `libstdc++-6.dll`, `libgcc_s_seh-1.dll`, and `libwinpthread-1.dll` next to `sms.exe`.

Run the backend directly:

```cmd
sms.exe
```

### Run the PyQt5 Frontend

```cmd
cd frontend
pip install -r requirements.txt
python main.py
```

The frontend auto-detects `sms.exe` in the project root, launches it as a subprocess, and provides a modern GUI over it. See [frontend/README.md](frontend/README.md) for full details.

## Default Login Credentials

| Role    | Username | Password |
|---------|----------|----------|
| Admin   | admin    | admin123 |

Additional accounts are created via the admin panel or `seed.py`.

## Usage

1. **First Run**: The system creates a default admin account and fee structures
2. **Login**: Use the credentials above to access the admin panel
3. **Setup**: Add teachers, students, and parents through the admin menu
4. **Daily Operations**: Teachers mark attendance, add grades, upload assignments
5. **Monitoring**: Parents and students can view academic progress

## Security Features

- **Password Hashing**: All passwords are securely hashed (Crypto.h)
- **Input Sanitization**: User inputs are validated
- **Role-Based Access Control**: Each role sees only its own menus
- **Account Lockout**: Protection against brute-force login attempts

## Technical Details

- **Language**: C++17
- **Architecture**: Object-oriented with inheritance
- **Data Storage**: File-based CSV persistence (`data/`)
- **Frontend**: Python 3 + PyQt5
- **Platform**: Windows (console with Unicode support)
- **C++ Dependencies**: Standard library only (no third-party libs)

## Troubleshooting

| Problem | Solution |
|---------|----------|
| `libstdc++-6.dll was not found` | Rebuild with `-static-libgcc -static-libstdc++`, or copy the MinGW DLLs next to `sms.exe` |
| Compiler not found | Install [MinGW-w64](https://www.mingw-w64.org/) and add it to PATH |
| Unicode characters not displaying | Ensure your terminal uses UTF-8 (`chcp 65001`) |
| Data not persisting | Check write permissions in the project directory |
| PyQt5 not found | Run `pip install -r frontend/requirements.txt` |

## License

Educational project — free to modify and use for learning purposes.
