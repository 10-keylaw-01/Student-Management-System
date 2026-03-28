# Student Management System - PyQt5 Frontend

Modern, professional GUI frontend for the C++ Student Management System backend.

## Prerequisites

- Python 3.8 or higher
- C++ backend compiled (see [main README](../README.md#how-to-build--run))

## Installation

### 1. Install Python Dependencies

```bash
cd frontend
pip install -r requirements.txt
```

### 2. Build C++ Backend (if not already built)

From the project root:

```bash
g++ -std=c++17 -o sms main.cpp
```

The executable should be in the root project directory or will be auto-detected.

## Running the Application

```bash
python main.py
```

The application will:
1. Auto-detect the C++ executable
2. Launch the login window
3. Start the backend subprocess automatically
4. Load the appropriate dashboard based on user role

## Project Structure

```
frontend/
├── main.py                    # Application entry point
├── backend_communicator.py    # Subprocess management for C++ backend
├── requirements.txt           # Python package dependencies
├── styles.qss                 # Modern Qt stylesheet
├── ui/                        # User interface modules
│   ├── __init__.py
│   ├── login_window.py        # Login with account lockout
│   ├── admin_dashboard.py     # Admin management interface
│   ├── teacher_dashboard.py   # Teacher grading & attendance
│   ├── student_dashboard.py   # Student progress view
│   └── parent_dashboard.py    # Parent progress monitoring
└── utils/                     # Utility modules
    ├── __init__.py
    ├── parser.py              # C++ output parsing
    └── widgets.py             # Custom PyQt5 components
```

## Architecture

### Backend Communication

The frontend communicates with the C++ backend via `BackendCommunicator`:

- Launches the C++ executable in a subprocess
- Sends commands via stdin
- Reads output via stdout in a separate thread (QThread)
- Prevents GUI freezing during backend operations

### Data Flow

```
User Input → PyQt5 Widget
         ↓
BackendCommunicator (sends command)
         ↓
C++ Backend (processes)
         ↓
BackendCommunicator (reads output)
         ↓
Output Parser (converts to structured data)
         ↓
GUI Display (tables, forms, cards)
```

## Features

### Login Window
- Username/password input
- Show/hide password toggle
- Failed attempt tracking (max 3 before 15-minute lockout)
- Account lockout status display
- Visual feedback for errors

### Role-Based Dashboards

#### Admin Dashboard
- **Students**: Add, edit, remove students
- **Teachers**: Add, edit, remove teachers
- **Fees**: View and set fee structure by class
- **Announcements**: Send school-wide announcements
- **Analytics**: View attendance and grade statistics

#### Teacher Dashboard
- **Mark Attendance**: Record daily attendance
- **Add Grades**: Enter and manage student grades
- **My Classes**: View assigned classes and student progress
- **Analytics**: Class attendance and performance metrics

#### Student Dashboard
- **Attendance**: View personal attendance records and percentage
- **Grades**: View all subject grades and scores
- **Fees**: View fee summary and payment history
- **Assignments**: View pending and completed assignments
- **Timetable**: View class schedule

#### Parent Dashboard
- **Child Progress**: Select child and view complete progress
- **Attendance**: Monitor attendance records
- **Grades**: View recent grades and scores
- **Fees**: View outstanding balance and payment history
- **Communications**: Read school announcements

### UI Components

#### Custom Widgets
- `DataTable`: Extended QTableWidget with auto-population from data
- `FormDialog`: Generic dialog for forms with auto-generated fields
- `PasswordLineEdit`: Password field with show/hide toggle
- `StatCard`: Summary card for displaying metrics

#### Styling
- Modern, professional flat design
- Color scheme: Blue (#2196F3) primary, Red (#f44336) for actions
- Responsive layout with proper spacing
- Alternating row colors in tables for readability

## Troubleshooting

### Backend Not Found
**Error**: "C++ backend executable not found"

**Solution**: Build the backend first:
```bash
g++ -std=c++17 -o sms main.cpp
```

### Module Import Errors
**Error**: "ModuleNotFoundError: No module named 'PyQt5'"

**Solution**: Install dependencies:
```bash
pip install -r requirements.txt
```

### Backend Crashes
**Error**: Application freezes or backend stops responding

**Solution**:
1. Check C++ backend builds without errors
2. Ensure data/ folder exists with CSV files
3. Try restarting the application

### GUI Looks Wrong
**Error**: Stylesheet not loading, widgets look unusual

**Solution**: 
1. Verify `styles.qss` exists in the frontend folder
2. Check file permissions
3. Restart the application

## Data Storage

The frontend reads/writes data through the C++ backend only. All CSV files are in the `data/` folder:

- `students.csv` - Student records
- `teachers.csv` - Teacher records
- `admins.csv` - Administrator accounts
- `parents.csv` - Parent information
- `attendance.csv` - Attendance records
- `grades.csv` - Grade records
- `fees.csv` - Fee payment history
- `fee_structure.csv` - Fee amounts by class
- ... and more

Never modify CSV files directly while the application is running.

## Development

### Adding a New Feature

1. Create UI in appropriate dashboard (`ui/dashboard_*.py`)
2. Add backend communication in `backend_communicator.py` if needed
3. For parsing C++ output, extend `utils/parser.py`
4. Use custom widgets from `utils/widgets.py`
5. Apply styles from `styles.qss` as needed

### Code Style

- Clean, well-documented code
- Follow PEP 8 style guidelines
- Use type hints where possible
- Keep functions concise and focused

### Testing

Test with all user roles:
- Admin: admin / admin123
- Teacher: t.ali / teach001
- Student: s.john / stu001
- Parent: p.mensah / par001

## Performance

- Threading prevents UI freezing
- Lazy loading of data (only display needed rows)
- Efficient CSV parsing
- Responsive even with 80+ students

## Future Improvements

- [ ] Database backend (currently CSV)
- [ ] Real-time notifications
- [ ] Export to PDF/Excel
- [ ] Online fee payment integration
- [ ] Mobile app companion
- [ ] Video conferencing for classes
- [ ] Offline mode support

## License

Same as main project.

## Support

For issues or questions, refer to the main [README](../README.md).
