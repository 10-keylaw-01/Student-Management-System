"""Login window — PyDracula dark style."""

import json
import csv
from datetime import datetime, timedelta
from pathlib import Path
from PyQt5.QtWidgets import (
    QWidget, QVBoxLayout, QHBoxLayout, QLabel,
    QPushButton, QLineEdit, QCheckBox, QProgressBar, QFrame
)
from PyQt5.QtCore import Qt, pyqtSignal
from backend_communicator import BackendCommunicator


def _fnv1a(password: str) -> str:
    FNV_PRIME  = 1099511628211
    FNV_OFFSET = 14695981039346656037
    h = FNV_OFFSET
    for c in password.encode():
        h ^= c
        h *= FNV_PRIME
        h = h % (2 ** 64)
    return format(h, 'x')


def _authenticate(data_dir: Path, username: str, password: str):
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


class LoginWindow(QWidget):
    """Login window — PyDracula dark two-panel card."""

    auth_success = pyqtSignal(str, str)

    def __init__(self, backend: BackendCommunicator):
        super().__init__()
        self.backend      = backend
        self.data_dir     = Path(__file__).parent.parent.parent / 'data' / 'private'
        self.lockout_file = Path(__file__).parent.parent.parent / '.lockout'
        self._setup_ui()

    def _setup_ui(self):
        self.setObjectName("loginPage")
        outer = QVBoxLayout(self)
        outer.setAlignment(Qt.AlignCenter)
        outer.setContentsMargins(0, 0, 0, 0)

        # ── Card ──────────────────────────────────────────────────
        card = QFrame()
        card.setObjectName("loginCard")
        card.setFixedSize(780, 460)
        card_layout = QHBoxLayout(card)
        card_layout.setContentsMargins(0, 0, 0, 0)
        card_layout.setSpacing(0)

        # Left branding panel
        left = QFrame()
        left.setObjectName("loginLeftPanel")
        left.setFixedWidth(300)
        left_layout = QVBoxLayout(left)
        left_layout.setAlignment(Qt.AlignCenter)
        left_layout.setContentsMargins(30, 40, 30, 40)
        left_layout.setSpacing(10)

        app_title = QLabel("SMS")
        app_title.setObjectName("loginAppTitle")
        app_title.setAlignment(Qt.AlignCenter)

        app_sub = QLabel("Student Management\nSystem")
        app_sub.setObjectName("loginAppSub")
        app_sub.setAlignment(Qt.AlignCenter)

        divider = QFrame()
        divider.setFrameShape(QFrame.HLine)
        divider.setStyleSheet("background-color: rgba(255,255,255,60); max-height: 1px;")

        tagline = QLabel("Manage students, teachers,\nfees and more — all in one place.")
        tagline.setObjectName("loginTagline")
        tagline.setAlignment(Qt.AlignCenter)
        tagline.setWordWrap(True)

        left_layout.addStretch()
        left_layout.addWidget(app_title)
        left_layout.addWidget(app_sub)
        left_layout.addSpacing(20)
        left_layout.addWidget(divider)
        left_layout.addSpacing(10)
        left_layout.addWidget(tagline)
        left_layout.addStretch()

        # Right form panel
        right = QFrame()
        right.setObjectName("loginRightPanel")
        right_layout = QVBoxLayout(right)
        right_layout.setContentsMargins(40, 40, 40, 40)
        right_layout.setSpacing(12)

        welcome = QLabel("Welcome Back")
        welcome.setObjectName("loginWelcome")

        sign_in = QLabel("Sign in to your account")
        sign_in.setObjectName("loginSubtitle")

        lbl_user = QLabel("Username")
        lbl_user.setObjectName("loginFieldLabel")
        self.username_input = QLineEdit()
        self.username_input.setPlaceholderText("Enter username")

        lbl_pwd = QLabel("Password")
        lbl_pwd.setObjectName("loginFieldLabel")
        pwd_row = QHBoxLayout()
        self.password_input = QLineEdit()
        self.password_input.setPlaceholderText("Enter password")
        self.password_input.setEchoMode(QLineEdit.Password)
        self.password_input.returnPressed.connect(self._attempt_login)
        self.show_pwd_check = QCheckBox("Show")
        self.show_pwd_check.stateChanged.connect(self._toggle_password)
        pwd_row.addWidget(self.password_input)
        pwd_row.addWidget(self.show_pwd_check)

        self.status_label = QLabel()
        self.status_label.setObjectName("loginStatus")
        self.status_label.setWordWrap(True)
        self.status_label.setMinimumHeight(18)

        self.login_btn = QPushButton("Login")
        self.login_btn.setObjectName("loginBtn")
        self.login_btn.clicked.connect(self._attempt_login)

        self.progress = QProgressBar()
        self.progress.setVisible(False)
        self.progress.setMaximum(0)
        self.progress.setFixedHeight(4)

        right_layout.addWidget(welcome)
        right_layout.addWidget(sign_in)
        right_layout.addSpacing(10)
        right_layout.addWidget(lbl_user)
        right_layout.addWidget(self.username_input)
        right_layout.addWidget(lbl_pwd)
        right_layout.addLayout(pwd_row)
        right_layout.addWidget(self.status_label)
        right_layout.addWidget(self.login_btn)
        right_layout.addWidget(self.progress)
        right_layout.addStretch()

        card_layout.addWidget(left)
        card_layout.addWidget(right)
        outer.addWidget(card)

    # ── Password toggle ───────────────────────────────────────────

    def _toggle_password(self):
        mode = QLineEdit.Normal if self.show_pwd_check.isChecked() else QLineEdit.Password
        self.password_input.setEchoMode(mode)

    # ── Lockout helpers ───────────────────────────────────────────

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
        data = self._load_lockout()
        if username in data and 'locked_until' in data[username]:
            until = datetime.fromisoformat(data[username]['locked_until'])
            remaining = (until - datetime.now()).total_seconds()
            if remaining > 0:
                return int(remaining)
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

    # ── Login flow ────────────────────────────────────────────────

    def _attempt_login(self):
        username = self.username_input.text().strip()
        password = self.password_input.text()

        if not username or not password:
            self.status_label.setText("Please enter both username and password.")
            return

        remaining = self._is_locked(username)
        if remaining > 0:
            mins, secs = divmod(remaining, 60)
            self.status_label.setText(f"🔒 Account locked. Try again in {mins}m {secs}s.")
            self.login_btn.setEnabled(False)
            return

        self.login_btn.setEnabled(False)
        self.progress.setVisible(True)
        self.status_label.setText("")

        role, name = _authenticate(self.data_dir, username, password)

        self.progress.setVisible(False)
        self.login_btn.setEnabled(True)

        if role:
            self._clear_lockout(username)
            self.auth_success.emit(username, role)
        else:
            attempts = self._record_failure(username)
            if attempts >= 3:
                self.login_btn.setEnabled(False)
                self.status_label.setText("🔒 Account locked for 15 minutes.")
            else:
                self.status_label.setText(
                    f"Invalid credentials. {3 - attempts} attempt(s) remaining."
                )
            self.password_input.clear()

    def reset(self):
        self.username_input.clear()
        self.password_input.clear()
        self.status_label.setText("")
        self.login_btn.setEnabled(True)
        self.show_pwd_check.setChecked(False)
