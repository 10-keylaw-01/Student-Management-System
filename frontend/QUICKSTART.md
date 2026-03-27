# Quick Reference - Student Management System Frontend

## Getting Started (Windows)

### Option 1: Automated Setup
```
1. Double-click: frontend/setup.bat
2. Double-click: frontend/start.bat
```

### Option 2: Manual Setup
```powershell
# Install dependencies
pip install -r frontend/requirements.txt

# Run application
python frontend/main.py
```

---

## Test Credentials

### Admin
- **Username**: admin
- **Password**: admin123
- **Access**: Full system management

### Teacher
- **Username**: t.ali
- **Password**: teach001
- **Access**: Mark attendance, add grades

### Student
- **Username**: s.john
- **Password**: stu001
- **Access**: View progress, fees, assignments

### Parent
- **Username**: p.mensah
- **Password**: par001
- **Access**: Monitor child's progress

---

## Key Features

### Login
- ✅ Username/password authentication
- ✅ Show/hide password toggle
- ✅ Failed attempt tracking (max 3)
- ✅ 15-minute account lockout
- ✅ Visual status display

### Admin Dashboard
- ✅ Manage students (add/edit/remove)
- ✅ Manage teachers
- ✅ Set fee structure by class
- ✅ Send announcements
- ✅ View analytics & statistics

### Teacher Dashboard
- ✅ Mark daily attendance
- ✅ Enter grades for students
- ✅ View assigned classes
- ✅ Class analytics

### Student Dashboard
- ✅ View attendance records & percentage
- ✅ View all grades by subject
- ✅ Check fee balance
- ✅ View assignments
- ✅ Check class timetable

### Parent Dashboard
- ✅ Select child to monitor
- ✅ View attendance
- ✅ View grades
- ✅ Track fees
- ✅ Read announcements

---

## File Organization

```
frontend/
├── main.py                    # Launch here
├── backend_communicator.py    # Backend interface
├── start.bat                  # Windows launcher
├── setup.bat                  # Windows setup
├── requirements.txt           # Dependencies
├── styles.qss                 # Modern styling
├── README.md                  # Full documentation
├── ui/                        # Dashboard modules
│   ├── login_window.py
│   ├── admin_dashboard.py
│   ├── teacher_dashboard.py
│   ├── student_dashboard.py
│   └── parent_dashboard.py
└── utils/                     # Helper utilities
    ├── parser.py
    └── widgets.py
```

---

## Troubleshooting

| Issue | Solution |
|-------|----------|
| "Backend not found" | Build C++ first: `g++ -std=c++17 -o sms main.cpp` |
| "No module PyQt5" | Install: `pip install -r frontend/requirements.txt` |
| App freezes | Restart application, ensure C++ builds without errors |
| Wrong styling | Delete `.pyc` files, restart application |
| Login fails | Check credentials in main README |

---

## Build C++ Backend

```bash
g++ -std=c++17 -o sms main.cpp
```

Generates: `sms.exe` (on Windows) or `sms` (on Unix)

---

## Architecture

```
PyQt5 GUI ←→ BackendCommunicator ←→ C++ Subprocess ←→ CSV Files
```

- Clean separation of concerns
- Non-blocking UI via threading
- Safe subprocess communication
- Persistent CSV storage

---

## Performance

- **Fast startup**: < 2 seconds
- **Responsive UI**: Threading prevents freezing
- **Efficient data**: CSV caching, lazy loading
- **Supports 80+ students** without lag

---

## Notes

- Never run multiple instances (only one backend at a time)
- Don't modify CSV files while app is running
- Close app gracefully to save all state
- Check data/ folder for CSV backups

---

For detailed documentation, see `frontend/README.md`
