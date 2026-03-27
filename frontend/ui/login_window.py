"""Login window for authentication."""

import json
import csv
from datetime import datetime, timedelta
from pathlib import Path
from PyQt5.QtWidgets import (
    QMainWindow, QWidget, QVBoxLayout, QHBoxLayout, QLabel,
    QPushButton, QLineEdit, QCheckBox, QMessageBox, QProgressBar
)
from PyQt5.QtCore import Qt, pyqtSignal
from backend_communicator import BackendCommunicator


def _fnv1a(password: str) -> str:
    """FNV-1a hash — matches C++ Crypto::hashPassword exactly."""
    FNV_PRIME  = 1099511628211
    FNV_OFFSET = 14695981039346656037
    h = FNV_OFFSET
    for c in password.encode():
        h ^= c
        h *= FNV_PRIME
        h = h % (2 ** 64)
    return format(h, 'x')


def _authenticate(data_dir: Path, username: str, password: str):
    """
    Verify credentials directly from CSV files.
    Returns (role, display_name) on success, or (None, None) on failure.
    CSV columns:
      students: id, username, password_hash, name, roll, class, parentId
      teachers: id, username, password_hash, name, subject, dept, classes
      admins:   id, username, password_hash, name
      parents:  id, username, password_hash, name, studentId
    """
    pwd_hash = _fnv1a(password)
    checks = [
        ('admins.csv',   'admin',   3),
        ('teachers.csv', 'teacher', 3),
        ('students.csv', 'student', 3),
        ('parents.csv',  'parent',  3),
    ]
    for filename, role, name_col in checks:
        path = data_dir / filename
        if not path.exists():
            continue
        try:
            with open(path, 'r', encoding='utf-8') as f:
                for row in csv.reader(f):
                    if len(row) > name_col and row[1] == username and row[2] == pwd_hash:
                        return role, row[name_col]
        except Exception:
            continue
    return None, None


class LoginWindow(QMainWindow):
    """Login window — authenticates directly from CSV, no subprocess timing."""

    auth_success = pyqtSignal(str, str)  # (username, role)

    def __init__(self, backend: BackendCommunicator):
        super().__init__()
        self.backend      = backend
        self.data_dir     = Path(__file__).parent.parent.parent / 'data'
        self.lockout_file = Path(__file__).parent.parent.parent / '.lockout'
        self._setup_ui()

    def _setup_ui(self):
        self.setWindowTitle("Student Management System - Login")
        self.setGeometry(100, 100, 450, 320)

        central = QWidget()
        self.setCentralWidget(central)
        layout = QVBoxLayout()
        layout.setContentsMargins(30, 30, 30, 30)
        layout.setSpacing(10)

        title = QLabel("School Management System")
        font  = title.font()
        font.setPointSize(16)
        font.setBold(True)
        title.setFont(font)
        title.setAlignment(Qt.AlignCenter)
        layout.addWidget(title)

        subtitle = QLabel("Please sign in to continue")
        subtitle.setAlignment(Qt.AlignCenter)
        subtitle.setStyleSheet("color: #666; margin-bottom: 10px;")
        layout.addWidget(subtitle)

        layout.addWidget(QLabel("Username:"))
        self.username_input = QLineEdit()
        self.username_input.setPlaceholderText("Enter username")
        self.username_input.setMinimumHeight(35)
        layout.addWidget(self.username_input)

        layout.addWidget(QLabel("Password:"))
        pwd_row = QHBoxLayout()
        self.password_input = QLineEdit()
        self.password_input.setPlaceholderText("Enter password")
        self.password_input.setEchoMode(QLineEdit.Password)
        self.password_input.setMinimumHeight(35)
        self.password_input.returnPressed.connect(self._attempt_login)

        self.show_pwd_check = QCheckBox("Show")
        self.show_pwd_check.stateChanged.connect(self._toggle_password)
        pwd_row.addWidget(self.password_input)
        pwd_row.addWidget(self.show_pwd_check)
        layout.addLayout(pwd_row)

        self.status_label = QLabel()
        self.status_label.setStyleSheet("color: #d32f2f;")
        self.status_label.setWordWrap(True)
        layout.addWidget(self.status_label)

        self.login_btn = QPushButton("Login")
        self.login_btn.setMinimumHeight(40)
        self.login_btn.clicked.connect(self._attempt_login)
        layout.addWidget(self.login_btn)

        self.progress = QProgressBar()
        self.progress.setVisible(False)
        layout.addWidget(self.progress)

        layout.addStretch()
        central.setLayout(layout)

    def _toggle_password(self):
        mode = QLineEdit.Normal if self.show_pwd_check.isChecked() else QLineEdit.Password
        self.password_input.setEchoMode(mode)

    # ── Lockout helpers ───────────────────────────────────────────────────────

    def _load_lockout(self) -> dict:
        try:
            if self.lockout_file.exists():
                with open(self.lockout_file, 'r') as f:
                    return json.load(f)
        except Exception:
            pass
        return {}

    def _save_lockout(self, data: dict):
        try:
            with open(self.lockout_file, 'w') as f:
                json.dump(data, f)
        except Exception:
            pass

    def _is_locked(self, username: str) -> int:
        """Return remaining seconds if locked, else 0."""
        data = self._load_lockout()
        if username in data and 'locked_until' in data[username]:
            until = datetime.fromisoformat(data[username]['locked_until'])
            remaining = (until - datetime.now()).total_seconds()
            if remaining > 0:
                return int(remaining)
            # Expired — clean up
            del data[username]
            self._save_lockout(data)
        return 0

    def _record_failure(self, username: str):
        data = self._load_lockout()
        entry = data.get(username, {'attempts': 0})
        entry['attempts'] = entry.get('attempts', 0) + 1
        if entry['attempts'] >= 3:
            entry['locked_until'] = (datetime.now() + timedelta(minutes=15)).isoformat()
        data[username] = entry
        self._save_lockout(data)
        return entry['attempts']

    def _clear_lockout(self, username: str):
        data = self._load_lockout()
        if username in data:
            del data[username]
            self._save_lockout(data)

    # ── Login flow ────────────────────────────────────────────────────────────

    def _attempt_login(self):
        username = self.username_input.text().strip()
        password = self.password_input.text()

        if not username or not password:
            self.status_label.setText("Please enter both username and password.")
            return

        # Check lockout
        remaining = self._is_locked(username)
        if remaining > 0:
            mins, secs = divmod(remaining, 60)
            self.status_label.setText(
                f"🔒 Account locked. Try again in {mins}m {secs}s."
            )
            self.login_btn.setEnabled(False)
            return

        self.login_btn.setEnabled(False)
        self.progress.setVisible(True)
        self.progress.setMaximum(0)
        self.status_label.setText("")

        role, name = _authenticate(self.data_dir, username, password)

        self.progress.setVisible(False)
        self.progress.setMaximum(100)
        self.login_btn.setEnabled(True)

        if role:
            self._clear_lockout(username)
            self.status_label.setText("")
            self.auth_success.emit(username, role)
        else:
            attempts = self._record_failure(username)
            if attempts >= 3:
                self.login_btn.setEnabled(False)
                self.status_label.setText(
                    "🔒 Account locked for 15 minutes due to too many failed attempts."
                )
            else:
                remaining_attempts = 3 - attempts
                self.status_label.setText(
                    f"Invalid credentials. {remaining_attempts} attempt(s) remaining."
                )
            self.password_input.clear()

    def reset(self):
        """Reset login form — called on logout."""
        self.username_input.clear()
        self.password_input.clear()
        self.status_label.setText("")
        self.login_btn.setEnabled(True)
        self.show_pwd_check.setChecked(False)

