"""Main entry point for SMS PyQt5 Frontend."""

import sys
from pathlib import Path

# Ensure the frontend directory is on the path so absolute imports work
sys.path.insert(0, str(Path(__file__).parent))

from PyQt5.QtWidgets import QApplication, QMainWindow, QStackedWidget, QMessageBox

from backend_communicator import BackendCommunicator
from ui.login_window import LoginWindow
from ui.admin_dashboard import AdminDashboard
from ui.teacher_dashboard import TeacherDashboard
from ui.student_dashboard import StudentDashboard
from ui.parent_dashboard import ParentDashboard


class MainWindow(QMainWindow):
    """Main application window."""

    def __init__(self, app: QApplication):
        super().__init__()
        self.app = app
        self.backend = None
        self.stack = QStackedWidget()
        self.setCentralWidget(self.stack)

        self.setWindowTitle("Student Management System")
        self.setGeometry(100, 100, 1000, 600)

        self._initialize_backend()
        self._setup_ui()

    def _initialize_backend(self):
        """Initialize backend communicator."""
        exe_paths = [
            Path(__file__).parent.parent / 'sms.exe',
            Path(__file__).parent.parent / 'main.exe',
            Path('sms.exe'),
            Path('./sms'),
        ]

        exe_path = None
        for path in exe_paths:
            if path.exists():
                exe_path = str(path)
                break

        if not exe_path:
            QMessageBox.critical(
                self,
                "Error",
                "C++ backend executable not found. Please build the C++ project first:\n\n"
                "g++ -std=c++17 -o sms main.cpp"
            )
            sys.exit(1)

        try:
            self.backend = BackendCommunicator(exe_path)
            self.backend.start()
        except Exception as e:
            QMessageBox.critical(self, "Error", f"Failed to start backend:\n{str(e)}")
            sys.exit(1)

    def _setup_ui(self):
        """Setup main UI with login."""
        self.login_window = LoginWindow(self.backend)
        self.login_window.auth_success.connect(self._on_auth_success)

        self.stack.addWidget(self.login_window)
        self.stack.setCurrentWidget(self.login_window)

    def _on_auth_success(self, username: str, role: str):
        """Handle successful authentication."""
        role = role.lower().strip()

        if self.stack.count() > 1:
            old_widget = self.stack.widget(1)
            self.stack.removeWidget(old_widget)

        if role == 'admin':
            dashboard = AdminDashboard(self.backend, username)
        elif role == 'teacher':
            dashboard = TeacherDashboard(self.backend, username)
        elif role == 'student':
            dashboard = StudentDashboard(self.backend, username)
        elif role == 'parent':
            dashboard = ParentDashboard(self.backend, username)
        else:
            QMessageBox.warning(
                self,
                "Unknown Role",
                f"Unknown user role: {role}\nPlease contact administrator."
            )
            return

        dashboard.logout.connect(self._on_logout)
        self.stack.addWidget(dashboard)
        self.stack.setCurrentWidget(dashboard)

    def _on_logout(self):
        """Handle logout."""
        self.stack.setCurrentWidget(self.login_window)
        self.login_window.reset()

    def closeEvent(self, event):
        """Clean up on close."""
        if self.backend:
            self.backend.stop()
        event.accept()


def main():
    """Main entry point."""
    app = QApplication(sys.argv)

    style_path = Path(__file__).parent / 'styles.qss'
    if style_path.exists():
        with open(style_path, 'r') as f:
            app.setStyleSheet(f.read())

    window = MainWindow(app)
    window.show()

    sys.exit(app.exec_())


if __name__ == '__main__':
    main()
