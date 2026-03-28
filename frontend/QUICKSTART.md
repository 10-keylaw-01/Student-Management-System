# Quick Reference — Student Management System

## Getting Started (Windows)

### Option 1: Automated Setup
```
1. Double-click: frontend/setup_simple.py   (or setup_desktop_app.py)
2. Double-click: frontend/start.bat         (to launch)
```

### Option 2: Manual
```powershell
cd frontend
python -m pip install -r requirements.txt
python main.py
```

---

## Login Credentials

### Admin
- **Username**: `admin`
- **Password**: `admin123`

### Teacher
- **Username**: `t.ali`
- **Password**: `teach001`
- Other teachers: `t.sara` / `teach002`, `t.james` / `teach003` … `t.samuel` / `teach010`

### Student
- **Username**: `s.akua1`
- **Password**: `stu001`
- Pattern: username = `s.<firstname><id>`, password = `stu<id padded to 3 digits>`
- Examples: `s.mary2` / `stu002`, `s.samuel10` / `stu010`

### Parent
- **Username**: `p.amoah1`
- **Password**: `par1`
- Pattern: username = `p.<lastname><id>`, password = `par<id>`

---

## Key Features

### Login
- Username/password authentication against CSV data
- Show/hide password toggle
- Failed attempt tracking (max 3 attempts)
- 15-minute account lockout after 3 failures

### Admin Dashboard
- Analytics: live stat cards + 4 charts (attendance pie, fee bar, enrollment line, class performance)
- Manage students (add / edit / remove)
- Manage teachers
- Fee structure by class
- Send announcements

### Teacher Dashboard
- Mark daily attendance
- Enter grades by class
- View assigned classes

### Student Dashboard
- Overview with live stat cards
- Attendance records and percentage bar
- Grades by subject and term
- Fee balance and structure
- Assignments, timetable, notifications

### Parent Dashboard
- Child overview with stat cards
- Attendance, grades, fees, announcements

---

## File Structure

```
frontend/
├── main.py                    # Entry point — run this
├── styles.qss                 # PyOneDark dark theme
├── requirements.txt           # PyQt5, matplotlib
├── start.bat                  # Quick launcher
├── setup_simple.py            # Setup + install script
├── launch_sms.bat             # Launcher with auto-install
├── ui/
│   ├── login_window.py
│   ├── dashboard_shell.py     # Shared sidebar shell
│   ├── admin_dashboard.py
│   ├── teacher_dashboard.py
│   ├── student_dashboard.py
│   ├── parent_dashboard.py
│   └── charts.py              # Matplotlib analytics charts
├── utils/
│   └── widgets.py             # DataTable, StatCard, FormDialog
└── modules/
    └── app_settings.py        # Animation / sidebar constants
```

---

## Troubleshooting

| Issue | Solution |
|-------|----------|
| `No module named PyQt5` | `python -m pip install PyQt5 matplotlib` |
| `Backend not found` | Build C++ first: `g++ -std=c++17 -o sms main.cpp` |
| Setup fails with C++ build error | Use `python -m pip install --only-binary :all: PyQt5 matplotlib` |
| App won't start | Make sure `sms.exe` or `main.exe` exists in the parent folder |
| Charts not showing | `python -m pip install matplotlib` |
| Login fails | Check credentials above — passwords are case-sensitive |
