"""Teacher dashboard — PyDracula style."""

import csv
from pathlib import Path
from PyQt5.QtWidgets import (
    QWidget, QVBoxLayout, QHBoxLayout, QPushButton, QLabel,
    QFormLayout, QComboBox, QDateEdit, QMessageBox, QSizePolicy
)
from PyQt5.QtCore import pyqtSignal, QDate

from backend_communicator import BackendCommunicator
from utils.widgets import DataTable, StatCard
from ui.dashboard_shell import DashboardShell


def _read_csv(path: Path) -> list:
    try:
        with open(path, 'r', encoding='utf-8', errors='replace') as f:
            return list(csv.reader(f))
    except Exception:
        return []


class TeacherDashboard(DashboardShell):
    """Teacher dashboard for attendance and grades."""

    def __init__(self, backend: BackendCommunicator, username: str):
        super().__init__(username, 'teacher')
        self.backend  = backend
        self.data_dir = Path(__file__).parent.parent.parent / 'data'
        self._csv: dict = {}
        self._build_pages()

    def _csv_rows(self, filename: str) -> list:
        if filename not in self._csv:
            self._csv[filename] = _read_csv(self.data_dir / filename)
        return self._csv[filename]

    def _build_pages(self):
        self.add_nav_button("📊", "Analytics",       self._create_analytics_tab())
        self.add_nav_button("✅", "Mark Attendance",  self._create_attendance_tab())
        self.add_nav_button("📝", "Add Grades",       self._create_grades_tab())
        self.add_nav_button("🏫", "My Classes",       self._create_classes_tab())

    # ── Pages ─────────────────────────────────────────────────────

    def _create_analytics_tab(self) -> QWidget:
        widget = QWidget()
        layout = QVBoxLayout(widget)
        layout.setContentsMargins(20, 20, 20, 20)
        layout.setSpacing(16)

        stats_row = QHBoxLayout()
        stats_row.setSpacing(12)
        stats_row.addWidget(StatCard("Classes Taught", "2"))
        stats_row.addWidget(StatCard("Total Students", "50"))
        stats_row.addWidget(StatCard("Avg Attendance", "88%"))
        stats_row.addWidget(StatCard("Avg Grade",      "85%"))
        layout.addLayout(stats_row)
        layout.addStretch()
        return widget

    def _create_attendance_tab(self) -> QWidget:
        widget = QWidget()
        layout = QVBoxLayout(widget)
        layout.setContentsMargins(20, 20, 20, 20)
        layout.setSpacing(12)

        date_form = QFormLayout()
        date_form.setSpacing(8)
        date_edit = QDateEdit(QDate.currentDate())
        date_edit.setCalendarPopup(True)
        date_form.addRow("Date:", date_edit)
        layout.addLayout(date_form)

        table = DataTable(['Student ID', 'Name', 'Status', 'Action'])
        self._populate_attendance_table(table)
        layout.addWidget(table)

        save_btn = QPushButton("Save Attendance")
        save_btn.setFixedWidth(160)
        layout.addWidget(save_btn)
        return widget

    def _create_grades_tab(self) -> QWidget:
        widget = QWidget()
        layout = QVBoxLayout(widget)
        layout.setContentsMargins(20, 20, 20, 20)
        layout.setSpacing(12)

        filter_row = QHBoxLayout()
        filter_row.setSpacing(8)
        filter_row.addWidget(QLabel("Class:"))
        class_combo = QComboBox()
        class_combo.addItems(['1A','1B','1C','2A','2B','2C','3A','3B','3C',
                              '4A','4B','4C','5A','5B','5C','6A','6B','6C'])
        filter_row.addWidget(class_combo)
        filter_row.addStretch()
        layout.addLayout(filter_row)

        table = DataTable(['Student', 'Subject', 'Score', 'Grade'])
        self._populate_grades_table(table)
        layout.addWidget(table)

        save_btn = QPushButton("Save Grades")
        save_btn.setFixedWidth(140)
        layout.addWidget(save_btn)
        return widget

    def _create_classes_tab(self) -> QWidget:
        widget = QWidget()
        layout = QVBoxLayout(widget)
        layout.setContentsMargins(20, 20, 20, 20)
        layout.setSpacing(12)

        lbl = QLabel("Assigned Classes")
        layout.addWidget(lbl)
        table = DataTable(['Class', 'Subject', 'Teacher'])
        self._populate_classes_table(table)
        layout.addWidget(table)
        return widget

    # ── CSV helpers ───────────────────────────────────────────────

    def _populate_attendance_table(self, table: DataTable):
        data = [
            {'Student ID': r[0], 'Name': r[3], 'Status': '', 'Action': ''}
            for r in self._csv_rows('students.csv') if len(r) >= 4
        ][:20]
        table.populate_from_list(data, ['Student ID', 'Name', 'Status', 'Action'])

    def _populate_grades_table(self, table: DataTable):
        data = [
            {'Student': r[1], 'Subject': r[2], 'Score': f"{float(r[4]):.1f}", 'Grade': ''}
            for r in self._csv_rows('grades.csv') if len(r) >= 5
        ][:20]
        table.populate_from_list(data, ['Student', 'Subject', 'Score', 'Grade'])

    def _populate_classes_table(self, table: DataTable):
        data = [
            {'Class': r[3], 'Subject': r[2], 'Teacher': r[5]}
            for r in self._csv_rows('courses.csv') if len(r) >= 6
        ]
        table.populate_from_list(data, ['Class', 'Subject', 'Teacher'])
