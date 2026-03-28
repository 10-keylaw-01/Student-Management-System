"""Admin dashboard — PyOneDark style with real analytics charts."""

import csv
from pathlib import Path

from PyQt5.QtWidgets import (
    QWidget, QVBoxLayout, QHBoxLayout, QPushButton, QLabel,
    QFormLayout, QLineEdit, QMessageBox, QTextEdit, QTableWidgetItem,
    QScrollArea, QFrame, QGridLayout, QComboBox, QSizePolicy
)
from PyQt5.QtCore import Qt

from backend_communicator import BackendCommunicator
from utils.widgets import DataTable, FormDialog, StatCard
from ui.dashboard_shell import DashboardShell
from data_paths import data_path
from ui.charts import (
    make_attendance_pie,
    make_fee_bar,
    make_class_performance_bar,
    make_enrollment_trend,
)

_ACCENT  = '#568af2'
_SUCCESS = '#98c379'
_WARNING = '#e5c07b'
_ERROR   = '#e06c75'
_CARD    = '#2c313a'


def _read_csv(path: Path) -> list:
    """Read a CSV safely with UTF-8 encoding."""
    try:
        with open(path, 'r', encoding='utf-8', errors='replace') as f:
            return list(csv.reader(f))
    except Exception:
        return []


class AdminDashboard(DashboardShell):
    """Admin dashboard with management features and analytics."""

    def __init__(self, backend: BackendCommunicator, username: str):
        super().__init__(username, 'admin')
        self.backend  = backend
        self.data_dir = None  # unused — paths resolved via data_path()
        self._csv: dict = {}   # CSV row cache — populated lazily
        self._build_pages()

    def _csv_rows(self, filename: str) -> list:
        """Return cached rows for a CSV file, reading once per session."""
        if filename not in self._csv:
            self._csv[filename] = _read_csv(data_path(filename))
        return self._csv[filename]

    def _build_pages(self):
        self.add_nav_button("📊", "Analytics",     self._create_analytics_tab())
        self.add_nav_button("🎓", "Students",      self._create_students_tab())
        self.add_nav_button("👩", "Teachers",      self._create_teachers_tab())
        self.add_nav_button("💰", "Fees",          self._create_fees_tab())
        self.add_nav_button("📢", "Announcements", self._create_announcements_tab())

    # ── Analytics ─────────────────────────────────────────────────

    def _create_analytics_tab(self) -> QWidget:
        outer = QWidget()
        outer.setObjectName("analyticsPage")

        scroll = QScrollArea()
        scroll.setWidgetResizable(True)
        scroll.setFrameShape(QFrame.NoFrame)
        scroll.setHorizontalScrollBarPolicy(Qt.ScrollBarAlwaysOff)

        inner = QWidget()
        layout = QVBoxLayout(inner)
        layout.setContentsMargins(20, 20, 20, 20)
        layout.setSpacing(18)

        # Toolbar
        toolbar = QHBoxLayout()
        toolbar.setSpacing(8)
        toolbar.setAlignment(Qt.AlignVCenter)

        title_lbl = QLabel("Analytics Overview")
        title_lbl.setObjectName("analyticsTitle")

        self._range_combo = QComboBox()
        self._range_combo.addItems(["All Time", "Last 90 Days", "Last 30 Days", "Last 7 Days"])
        self._range_combo.setFixedSize(150, 34)
        self._range_combo.setToolTip("Filter date range")

        refresh_btn = QPushButton("⟳  Refresh")
        refresh_btn.setObjectName("refreshBtn")
        refresh_btn.setFixedSize(100, 34)
        refresh_btn.setToolTip("Reload charts from data")
        refresh_btn.clicked.connect(self._refresh_analytics)

        toolbar.addWidget(title_lbl)
        toolbar.addStretch()
        toolbar.addWidget(self._range_combo)
        toolbar.addWidget(refresh_btn)
        layout.addLayout(toolbar)

        # Stat cards
        stats = self._load_summary_stats()
        cards_row = QHBoxLayout()
        cards_row.setSpacing(12)
        cards_row.addWidget(StatCard("Total Students", str(stats['students']), accent=_ACCENT))
        cards_row.addWidget(StatCard("Total Teachers", str(stats['teachers']), accent=_SUCCESS))
        cards_row.addWidget(StatCard("Avg Attendance", stats['attendance'],    accent=_WARNING))
        cards_row.addWidget(StatCard("Fee Collection", stats['fee_pct'],       accent=_ERROR))
        layout.addLayout(cards_row)

        # Charts 2×2
        self._chart_grid = QGridLayout()
        self._chart_grid.setSpacing(14)
        self._chart_grid.setColumnStretch(0, 1)
        self._chart_grid.setColumnStretch(1, 1)
        layout.addLayout(self._chart_grid)
        self._render_charts()

        scroll.setWidget(inner)
        page_layout = QVBoxLayout(outer)
        page_layout.setContentsMargins(0, 0, 0, 0)
        page_layout.addWidget(scroll)
        return outer

    def _chart_card(self, canvas) -> QFrame:
        card = QFrame()
        card.setObjectName("chartCard")
        card.setStyleSheet(
            f"#chartCard {{ background-color: {_CARD}; border-radius: 10px;"
            f" border: 1px solid #3a3f4b; }}"
        )
        vbox = QVBoxLayout(card)
        vbox.setContentsMargins(8, 8, 8, 8)
        vbox.addWidget(canvas)
        return card

    def _render_charts(self):
        while self._chart_grid.count():
            item = self._chart_grid.takeAt(0)
            if item.widget():
                item.widget().deleteLater()
        charts = [
            make_attendance_pie(),
            make_fee_bar(),
            make_enrollment_trend(),
            make_class_performance_bar(),
        ]
        for canvas, (row, col) in zip(charts, [(0,0),(0,1),(1,0),(1,1)]):
            self._chart_grid.addWidget(self._chart_card(canvas), row, col)

    def _refresh_analytics(self):
        self._render_charts()

    def _load_summary_stats(self) -> dict:
        students = sum(1 for r in self._csv_rows('students.csv')  if len(r) >= 2)
        teachers = sum(1 for r in self._csv_rows('teachers.csv')  if len(r) >= 2)

        att_rows = [r for r in self._csv_rows('attendance.csv') if len(r) >= 5]
        present  = sum(1 for r in att_rows if r[4] == '1')
        att_pct  = f"{int(100 * present / len(att_rows))}%" if att_rows else 'N/A'

        fee_rows = [r for r in self._csv_rows('fees.csv') if len(r) >= 8]
        owed = sum(float(r[6]) for r in fee_rows)
        paid = sum(float(r[7]) for r in fee_rows)
        fee_pct = f"{int(100 * paid / owed)}%" if owed else 'N/A'

        return {'students': students, 'teachers': teachers,
                'attendance': att_pct, 'fee_pct': fee_pct}

    # ── Students ──────────────────────────────────────────────────

    def _create_students_tab(self) -> QWidget:
        widget = QWidget()
        layout = QVBoxLayout(widget)
        layout.setContentsMargins(20, 20, 20, 20)
        layout.setSpacing(12)

        btn_row = QHBoxLayout()
        btn_row.setSpacing(8)
        add_btn    = QPushButton("+ Add Student")
        edit_btn   = QPushButton("✎ Edit Selected")
        remove_btn = QPushButton("✕ Remove Selected")
        add_btn.clicked.connect(self._add_student)
        edit_btn.clicked.connect(self._edit_student)
        remove_btn.clicked.connect(self._remove_student)
        btn_row.addWidget(add_btn)
        btn_row.addWidget(edit_btn)
        btn_row.addWidget(remove_btn)
        btn_row.addStretch()
        layout.addLayout(btn_row)

        self.students_table = DataTable(['ID', 'Name', 'Class', 'Username'])
        self._populate_students_table(self.students_table)
        self.students_table.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
        layout.addWidget(self.students_table, 1)
        return widget

    # ── Teachers ──────────────────────────────────────────────────

    def _create_teachers_tab(self) -> QWidget:
        widget = QWidget()
        layout = QVBoxLayout(widget)
        layout.setContentsMargins(20, 20, 20, 20)
        layout.setSpacing(12)

        btn_row = QHBoxLayout()
        btn_row.setSpacing(8)
        add_btn    = QPushButton("+ Add Teacher")
        edit_btn   = QPushButton("✎ Edit Selected")
        remove_btn = QPushButton("✕ Remove Selected")
        add_btn.clicked.connect(self._add_teacher)
        edit_btn.clicked.connect(self._edit_teacher)
        remove_btn.clicked.connect(self._remove_teacher)
        btn_row.addWidget(add_btn)
        btn_row.addWidget(edit_btn)
        btn_row.addWidget(remove_btn)
        btn_row.addStretch()
        layout.addLayout(btn_row)

        self.teachers_table = DataTable(['ID', 'Name', 'Subject', 'Username'])
        self._populate_teachers_table(self.teachers_table)
        self.teachers_table.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
        layout.addWidget(self.teachers_table, 1)
        return widget

    # ── Fees ──────────────────────────────────────────────────────

    def _create_fees_tab(self) -> QWidget:
        widget = QWidget()
        layout = QVBoxLayout(widget)
        layout.setContentsMargins(20, 20, 20, 20)
        layout.setSpacing(12)

        lbl = QLabel("Fee Structure by Class")
        lbl.setObjectName("sectionLabel")
        layout.addWidget(lbl)

        table = DataTable(['Class', 'Amount (GHS)'])
        self._populate_fees_table(table)
        table.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
        layout.addWidget(table, 1)
        return widget

    # ── Announcements ─────────────────────────────────────────────

    def _create_announcements_tab(self) -> QWidget:
        widget = QWidget()
        layout = QVBoxLayout(widget)
        layout.setContentsMargins(20, 20, 20, 20)
        layout.setSpacing(12)

        form = QFormLayout()
        form.setSpacing(10)
        form.setLabelAlignment(Qt.AlignRight)
        self.announce_title = QLineEdit()
        self.announce_title.setPlaceholderText("Announcement title...")
        self.announce_content = QTextEdit()
        self.announce_content.setPlaceholderText("Write your announcement here...")
        self.announce_content.setFixedHeight(120)
        form.addRow("Title:", self.announce_title)
        form.addRow("Content:", self.announce_content)

        send_btn = QPushButton("  📢  Send Announcement  ")
        send_btn.setObjectName("sendAnnouncementBtn")
        send_btn.clicked.connect(self._send_announcement)

        layout.addLayout(form)
        layout.addWidget(send_btn)
        layout.addStretch()
        return widget

    # ── Student handlers ──────────────────────────────────────────

    def _add_student(self):
        dialog = FormDialog("Add Student", [
            ("ID", "text"), ("Name", "text"), ("Class", "text"), ("Username", "text")
        ], self)
        if dialog.exec_() == FormDialog.Accepted:
            values = dialog.get_values()
            row = self.students_table.rowCount()
            self.students_table.setRowCount(row + 1)
            for col, key in enumerate(["ID", "Name", "Class", "Username"]):
                self.students_table.setItem(row, col, QTableWidgetItem(values.get(key, '')))

    def _edit_student(self):
        row_data = self.students_table.get_selected_row()
        if not row_data:
            QMessageBox.warning(self, "No Selection", "Please select a student to edit.")
            return
        dialog = FormDialog("Edit Student", [
            ("ID", "text"), ("Name", "text"), ("Class", "text"), ("Username", "text")
        ], self)
        dialog.set_values(row_data)
        if dialog.exec_() == FormDialog.Accepted:
            values = dialog.get_values()
            cur = self.students_table.currentRow()
            for col, key in enumerate(["ID", "Name", "Class", "Username"]):
                self.students_table.setItem(cur, col, QTableWidgetItem(values.get(key, '')))

    def _remove_student(self):
        row_data = self.students_table.get_selected_row()
        if not row_data:
            QMessageBox.warning(self, "No Selection", "Please select a student to remove.")
            return
        if QMessageBox.question(self, "Confirm",
                                f"Remove '{row_data.get('Name', '')}'?") == QMessageBox.Yes:
            self.students_table.removeRow(self.students_table.currentRow())

    # ── Teacher handlers ──────────────────────────────────────────

    def _add_teacher(self):
        dialog = FormDialog("Add Teacher", [
            ("ID", "text"), ("Name", "text"), ("Subject", "text"), ("Username", "text")
        ], self)
        if dialog.exec_() == FormDialog.Accepted:
            values = dialog.get_values()
            row = self.teachers_table.rowCount()
            self.teachers_table.setRowCount(row + 1)
            for col, key in enumerate(["ID", "Name", "Subject", "Username"]):
                self.teachers_table.setItem(row, col, QTableWidgetItem(values.get(key, '')))

    def _edit_teacher(self):
        row_data = self.teachers_table.get_selected_row()
        if not row_data:
            QMessageBox.warning(self, "No Selection", "Please select a teacher to edit.")
            return
        dialog = FormDialog("Edit Teacher", [
            ("ID", "text"), ("Name", "text"), ("Subject", "text"), ("Username", "text")
        ], self)
        dialog.set_values(row_data)
        if dialog.exec_() == FormDialog.Accepted:
            values = dialog.get_values()
            cur = self.teachers_table.currentRow()
            for col, key in enumerate(["ID", "Name", "Subject", "Username"]):
                self.teachers_table.setItem(cur, col, QTableWidgetItem(values.get(key, '')))

    def _remove_teacher(self):
        row_data = self.teachers_table.get_selected_row()
        if not row_data:
            QMessageBox.warning(self, "No Selection", "Please select a teacher to remove.")
            return
        if QMessageBox.question(self, "Confirm",
                                f"Remove '{row_data.get('Name', '')}'?") == QMessageBox.Yes:
            self.teachers_table.removeRow(self.teachers_table.currentRow())

    # ── Announcement handler ──────────────────────────────────────

    def _send_announcement(self):
        title   = self.announce_title.text().strip()
        content = self.announce_content.toPlainText().strip()
        if not title or not content:
            QMessageBox.warning(self, "Input Error", "Please enter both title and content.")
            return
        QMessageBox.information(self, "Sent", f"Announcement '{title}' sent successfully.")
        self.announce_title.clear()
        self.announce_content.clear()

    # ── CSV helpers ───────────────────────────────────────────────

    def _populate_students_table(self, table: DataTable):
        data = [
            {'ID': r[0], 'Name': r[3], 'Class': r[5], 'Username': r[1]}
            for r in self._csv_rows('students.csv') if len(r) >= 6
        ]
        if not data:
            QMessageBox.warning(self, "Error", "Failed to load students.")
        table.populate_from_list(data, ['ID', 'Name', 'Class', 'Username'])

    def _populate_teachers_table(self, table: DataTable):
        data = [
            {'ID': r[0], 'Name': r[3], 'Subject': r[4], 'Username': r[1]}
            for r in self._csv_rows('teachers.csv') if len(r) >= 5
        ]
        table.populate_from_list(data, ['ID', 'Name', 'Subject', 'Username'])

    def _populate_fees_table(self, table: DataTable):
        data = [
            {'Class': r[0], 'Amount (GHS)': f"{float(r[1]):.2f}"}
            for r in self._csv_rows('fee_structure.csv') if len(r) >= 2
        ]
        table.populate_from_list(data, ['Class', 'Amount (GHS)'])
