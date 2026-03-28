"""Main entry point for SMS PyQt5 Frontend — PyDracula style."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))

from PyQt5.QtWidgets import (
    QApplication, QMainWindow, QWidget, QStackedWidget,
    QMessageBox, QHBoxLayout, QVBoxLayout, QLabel,
    QPushButton, QFrame, QSizeGrip, QGraphicsDropShadowEffect
)
from PyQt5.QtCore import Qt
from PyQt5.QtGui import QColor, QIcon, QFont

from backend_communicator import BackendCommunicator
from ui.login_window import LoginWindow
from ui.admin_dashboard import AdminDashboard
from ui.teacher_dashboard import TeacherDashboard
from ui.student_dashboard import StudentDashboard
from ui.parent_dashboard import ParentDashboard


class MainWindow(QMainWindow):
    """Frameless main window with PyDracula-style chrome."""

    def __init__(self, app: QApplication):
        super().__init__()
        self.app = app
        self.backend = None
        self._drag_pos = None
        self._is_maximized = False

        self.setWindowTitle("Student Management System")
        self.setMinimumSize(960, 580)
        self.resize(1100, 660)

        # Set window icon
        icon_path = Path(__file__).parent / 'resources' / 'icons' / 'app_icon.png'
        if icon_path.exists():
            self.setWindowIcon(QIcon(str(icon_path)))

        # Frameless + translucent for drop shadow
        self.setWindowFlags(Qt.FramelessWindowHint)
        self.setAttribute(Qt.WA_TranslucentBackground)

        self._build_shell()
        self._initialize_backend()
        self._setup_ui()

    # ── Shell (outer frame + title bar) ──────────────────────────

    def _build_shell(self):
        """Build the outer rounded frame with title bar and content area."""
        # Outer widget (transparent — only for shadow)
        outer = QWidget()
        outer.setObjectName("outerWidget")
        self.setCentralWidget(outer)
        outer_layout = QVBoxLayout(outer)
        outer_layout.setContentsMargins(10, 10, 10, 10)
        outer_layout.setSpacing(0)

        # Main app frame
        self.bgApp = QFrame()
        self.bgApp.setObjectName("bgApp")
        self.bgApp.setStyleSheet(
            "#bgApp { background-color: rgb(40,44,52); border: 1px solid rgb(44,49,58); border-radius: 8px; }"
        )
        outer_layout.addWidget(self.bgApp)

        # Drop shadow
        shadow = QGraphicsDropShadowEffect(self)
        shadow.setBlurRadius(20)
        shadow.setXOffset(0)
        shadow.setYOffset(0)
        shadow.setColor(QColor(0, 0, 0, 160))
        self.bgApp.setGraphicsEffect(shadow)

        app_layout = QVBoxLayout(self.bgApp)
        app_layout.setContentsMargins(0, 0, 0, 0)
        app_layout.setSpacing(0)

        # ── Title bar ──
        title_bar = QFrame()
        title_bar.setObjectName("contentTopBg")
        title_bar.setFixedHeight(50)
        title_bar.setStyleSheet("#contentTopBg { background-color: rgb(33,37,43); border-top-left-radius: 8px; border-top-right-radius: 8px; }")
        tb_layout = QHBoxLayout(title_bar)
        tb_layout.setContentsMargins(15, 0, 10, 0)
        tb_layout.setSpacing(5)

        self.title_label = QLabel("Student Management System")
        self.title_label.setObjectName("titleRightInfo")
        self.title_label.setStyleSheet("color: rgb(221,221,221); font: 10pt 'Segoe UI';")

        # Window control buttons
        btn_frame = QFrame()
        btn_frame.setObjectName("rightButtons")
        btn_layout = QHBoxLayout(btn_frame)
        btn_layout.setContentsMargins(0, 0, 0, 0)
        btn_layout.setSpacing(5)

        self.min_btn  = self._make_ctrl_btn("─", self.showMinimized)
        self.max_btn  = self._make_ctrl_btn("□", self._toggle_maximize)
        self.close_btn = self._make_ctrl_btn("✕", self.close)
        self.close_btn.setStyleSheet(
            self.close_btn.styleSheet() +
            "QPushButton:hover { background-color: rgb(196,43,28); }"
        )

        btn_layout.addWidget(self.min_btn)
        btn_layout.addWidget(self.max_btn)
        btn_layout.addWidget(self.close_btn)

        tb_layout.addWidget(self.title_label)
        tb_layout.addStretch()
        tb_layout.addWidget(btn_frame)

        # Enable window drag via title bar
        title_bar.mousePressEvent   = self._title_mouse_press
        title_bar.mouseMoveEvent    = self._title_mouse_move
        title_bar.mouseDoubleClickEvent = lambda e: self._toggle_maximize()

        # ── Content stack ──
        self.stack = QStackedWidget()
        self.stack.setObjectName("pagesContainer")

        # ── Bottom bar ──
        bottom_bar = QFrame()
        bottom_bar.setObjectName("bottomBar")
        bottom_bar.setFixedHeight(22)
        bottom_bar.setStyleSheet(
            "#bottomBar { background-color: rgb(44,49,58); border-bottom-left-radius: 8px; border-bottom-right-radius: 8px; }"
        )
        bb_layout = QHBoxLayout(bottom_bar)
        bb_layout.setContentsMargins(10, 0, 10, 0)

        credits = QLabel("Student Management System")
        credits.setStyleSheet("color: rgb(113,126,149); font-size: 10px;")
        version = QLabel("v1.0.0")
        version.setStyleSheet("color: rgb(113,126,149); font-size: 10px;")
        version.setAlignment(Qt.AlignRight)

        size_grip_frame = QFrame()
        size_grip_frame.setFixedWidth(20)
        self.size_grip = QSizeGrip(size_grip_frame)
        self.size_grip.setStyleSheet("width:20px; height:20px; margin:0; padding:0;")

        bb_layout.addWidget(credits)
        bb_layout.addStretch()
        bb_layout.addWidget(version)
        bb_layout.addWidget(size_grip_frame)

        app_layout.addWidget(title_bar)
        app_layout.addWidget(self.stack, 1)
        app_layout.addWidget(bottom_bar)

    def _make_ctrl_btn(self, text: str, slot) -> QPushButton:
        btn = QPushButton(text)
        btn.setFixedSize(28, 28)
        btn.setCursor(Qt.PointingHandCursor)
        btn.setStyleSheet(
            "QPushButton { background-color: rgba(255,255,255,0); border: none; border-radius: 5px; color: rgb(180,180,180); font: 10pt 'Segoe UI'; }"
            "QPushButton:hover { background-color: rgb(44,49,57); }"
            "QPushButton:pressed { background-color: rgb(23,26,30); }"
        )
        btn.clicked.connect(slot)
        return btn

    # ── Window drag / maximize ────────────────────────────────────

    def _title_mouse_press(self, event):
        if event.button() == Qt.LeftButton:
            self._drag_pos = event.globalPos() - self.frameGeometry().topLeft()

    def _title_mouse_move(self, event):
        if event.buttons() == Qt.LeftButton and self._drag_pos:
            if self._is_maximized:
                self._toggle_maximize()
            self.move(event.globalPos() - self._drag_pos)

    def _toggle_maximize(self):
        if self._is_maximized:
            self.showNormal()
            self._is_maximized = False
            self.bgApp.setStyleSheet(
                "#bgApp { background-color: rgb(40,44,52); border: 1px solid rgb(44,49,58); border-radius: 8px; }"
            )
        else:
            self.showMaximized()
            self._is_maximized = True
            self.bgApp.setStyleSheet(
                "#bgApp { background-color: rgb(40,44,52); border: 1px solid rgb(44,49,58); border-radius: 0px; }"
            )

    # ── Backend ───────────────────────────────────────────────────

    def _initialize_backend(self):
        exe_paths = [
            Path(__file__).parent.parent / 'sms',
            Path(__file__).parent.parent / 'main',
            Path(__file__).parent.parent / 'sms.exe',
            Path(__file__).parent.parent / 'main.exe',
        ]
        exe_path = next((str(p) for p in exe_paths if p.exists()), None)

        if not exe_path:
            QMessageBox.critical(
                self, "Error",
                "C++ backend executable not found.\nPlease build the C++ project first."
            )
            sys.exit(1)

        try:
            self.backend = BackendCommunicator(exe_path)
            self.backend.start()
        except Exception as e:
            QMessageBox.critical(self, "Error", f"Failed to start backend:\n{str(e)}")
            sys.exit(1)

    # ── UI setup ──────────────────────────────────────────────────

    def _setup_ui(self):
        self.login_window = LoginWindow(self.backend)
        self.login_window.auth_success.connect(self._on_auth_success)
        self.stack.addWidget(self.login_window)
        self.stack.setCurrentWidget(self.login_window)

    def _on_auth_success(self, username: str, role: str):
        role = role.lower().strip()

        if self.stack.count() > 1:
            old = self.stack.widget(1)
            self.stack.removeWidget(old)
            old.deleteLater()

        dashboard_map = {
            'admin':   AdminDashboard,
            'teacher': TeacherDashboard,
            'student': StudentDashboard,
            'parent':  ParentDashboard,
        }

        cls = dashboard_map.get(role)
        if not cls:
            QMessageBox.warning(self, "Unknown Role", f"Unknown role: {role}")
            return

        dashboard = cls(self.backend, username)
        dashboard.logout.connect(self._on_logout)
        self.stack.addWidget(dashboard)
        self.stack.setCurrentWidget(dashboard)
        self.title_label.setText(f"Student Management System  —  {role.capitalize()}: {username}")

    def _on_logout(self):
        self.stack.setCurrentWidget(self.login_window)
        self.login_window.reset()
        self.title_label.setText("Student Management System")

    def closeEvent(self, event):
        if self.backend:
            self.backend.stop()
        event.accept()


def main():
    app = QApplication(sys.argv)
    app.setFont(QFont("Segoe UI", 10))
    app.setApplicationName("Student Management System")
    app.setApplicationVersion("1.0.0")

    # Set app icon
    icon_path = Path(__file__).parent / 'resources' / 'icons' / 'app_icon.png'
    if icon_path.exists():
        app.setWindowIcon(QIcon(str(icon_path)))

    style_path = Path(__file__).parent / 'styles.qss'
    if style_path.exists():
        app.setStyleSheet(style_path.read_text(encoding='utf-8'))

    window = MainWindow(app)
    window.show()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()
