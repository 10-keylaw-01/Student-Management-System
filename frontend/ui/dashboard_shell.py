"""Shared PyDracula-style dashboard shell for all roles."""

from PyQt5.QtWidgets import (
    QWidget, QHBoxLayout, QVBoxLayout, QPushButton,
    QLabel, QFrame, QStackedWidget, QScrollArea
)
from PyQt5.QtCore import Qt, QPropertyAnimation, QParallelAnimationGroup, QEasingCurve, pyqtSignal

from modules.app_settings import Settings


class DashboardShell(QWidget):
    """
    PyDracula-style shell: collapsible left sidebar + stacked content area.
    Subclasses call add_nav_button(label, icon_char, page_widget) to populate.
    """

    logout = pyqtSignal()

    def __init__(self, username: str, role: str, parent=None):
        super().__init__(parent)
        self.username = username
        self.role = role
        self._nav_buttons = []
        self._expanded = False
        self._build_shell()

    def _build_shell(self):
        root = QHBoxLayout(self)
        root.setContentsMargins(0, 0, 0, 0)
        root.setSpacing(0)

        # ── Left sidebar ──────────────────────────────────────────
        self.sidebar = QFrame()
        self.sidebar.setObjectName("leftMenuBg")
        self.sidebar.setFixedWidth(Settings.MENU_WIDTH_COLLAPSED)

        sidebar_vbox = QVBoxLayout(self.sidebar)
        sidebar_vbox.setContentsMargins(0, 0, 0, 0)
        sidebar_vbox.setSpacing(0)

        # Toggle button — shows only icon when collapsed
        self.toggle_btn = QPushButton("☰")
        self.toggle_btn.setObjectName("toggleButton")
        self.toggle_btn.setFixedHeight(48)
        self.toggle_btn.setCursor(Qt.PointingHandCursor)
        self.toggle_btn.setToolTip("Toggle menu")
        self.toggle_btn.clicked.connect(self._toggle_menu)

        # User info — only visible when expanded
        self.user_label = QLabel(f"{self.role.capitalize()}\n{self.username}")
        self.user_label.setObjectName("userInfoLabel")
        self.user_label.setAlignment(Qt.AlignLeft | Qt.AlignVCenter)
        self.user_label.setVisible(False)
        self.user_label.setWordWrap(True)

        # Nav scroll area (so many buttons don't overflow)
        nav_scroll = QScrollArea()
        nav_scroll.setWidgetResizable(True)
        nav_scroll.setFrameShape(QFrame.NoFrame)
        nav_scroll.setHorizontalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
        nav_scroll.setVerticalScrollBarPolicy(Qt.ScrollBarAsNeeded)
        nav_scroll.setStyleSheet("background: transparent; border: none;")

        self.nav_container = QWidget()
        self.nav_container.setStyleSheet("background: transparent;")
        self.nav_layout = QVBoxLayout(self.nav_container)
        self.nav_layout.setContentsMargins(0, 0, 0, 0)
        self.nav_layout.setSpacing(0)
        self.nav_layout.addStretch()
        nav_scroll.setWidget(self.nav_container)

        # Logout button
        self.logout_btn = QPushButton("⏻")
        self.logout_btn.setObjectName("logoutNavBtn")
        self.logout_btn.setFixedHeight(46)
        self.logout_btn.setCursor(Qt.PointingHandCursor)
        self.logout_btn.setToolTip("Logout")
        self.logout_btn.clicked.connect(self.logout.emit)

        sidebar_vbox.addWidget(self.toggle_btn)
        sidebar_vbox.addWidget(self.user_label)
        sidebar_vbox.addWidget(nav_scroll, 1)
        sidebar_vbox.addWidget(self.logout_btn)

        # ── Content area ──────────────────────────────────────────
        content_box = QFrame()
        content_box.setObjectName("stackedWidget")

        content_vbox = QVBoxLayout(content_box)
        content_vbox.setContentsMargins(0, 0, 0, 0)
        content_vbox.setSpacing(0)

        # Page title bar
        self.page_title_bar = QFrame()
        self.page_title_bar.setObjectName("pageTitleBar")
        self.page_title_bar.setFixedHeight(44)

        ptb_hbox = QHBoxLayout(self.page_title_bar)
        ptb_hbox.setContentsMargins(20, 0, 20, 0)

        self.page_title_label = QLabel("Dashboard")
        self.page_title_label.setObjectName("pageTitleLabel")

        ptb_hbox.addWidget(self.page_title_label)
        ptb_hbox.addStretch()

        # Stacked pages
        self.pages = QStackedWidget()

        content_vbox.addWidget(self.page_title_bar)
        content_vbox.addWidget(self.pages, 1)

        root.addWidget(self.sidebar)
        root.addWidget(content_box, 1)

    # ── Public API ────────────────────────────────────────────────

    def add_nav_button(self, icon: str, label: str, page: QWidget):
        """
        Add a nav button.
        icon  — single emoji/char shown when collapsed
        label — full text shown when expanded
        page  — the QWidget to display
        """
        btn = QPushButton(icon)
        btn.setObjectName("navBtn")
        btn.setFixedHeight(46)
        btn.setCursor(Qt.PointingHandCursor)
        btn.setCheckable(True)
        btn.setToolTip(label)

        idx = self.pages.count()
        self.pages.addWidget(page)
        self._nav_buttons.append((btn, icon, label))

        btn.clicked.connect(lambda _checked, i=idx, lbl=label: self._switch_page(i, lbl))

        # Insert before the trailing stretch
        self.nav_layout.insertWidget(self.nav_layout.count() - 1, btn)

        if idx == 0:
            btn.setChecked(True)
            self.page_title_label.setText(label)

    # ── Internals ─────────────────────────────────────────────────

    def _switch_page(self, index: int, label: str):
        self.pages.setCurrentIndex(index)
        self.page_title_label.setText(label)
        for i, (b, _icon, _lbl) in enumerate(self._nav_buttons):
            b.setChecked(i == index)

    def _toggle_menu(self):
        self._expanded = not self._expanded
        target = Settings.MENU_WIDTH if self._expanded else Settings.MENU_WIDTH_COLLAPSED
        current = self.sidebar.width()

        # Animate min and max width in parallel so they stay in sync
        self._anim_group = QParallelAnimationGroup()
        for prop in (b"minimumWidth", b"maximumWidth"):
            anim = QPropertyAnimation(self.sidebar, prop)
            anim.setDuration(Settings.TIME_ANIMATION)
            anim.setStartValue(current)
            anim.setEndValue(target)
            anim.setEasingCurve(QEasingCurve.OutCubic)
            self._anim_group.addAnimation(anim)
        self._anim_group.start()

        # Update labels immediately — they clip naturally while sidebar is narrow
        self.user_label.setVisible(self._expanded)
        self.toggle_btn.setText("✕" if self._expanded else "☰")
        self.logout_btn.setText("⏻  Logout" if self._expanded else "⏻")
        for btn, icon, label in self._nav_buttons:
            btn.setText(f"  {icon}  {label}" if self._expanded else icon)
            btn.setToolTip("" if self._expanded else label)
