"""Teacher dashboard — PyDracula style."""

import csv
from pathlib import Path
from PyQt5.QtWidgets import (
    QWidget, QVBoxLayout, QHBoxLayout, QPushButton, QLabel,
    QFormLayout, QComboBox, QDateEdit
)
from PyQt5.QtCore import QDate

from backend_communicator import BackendCommunicator
from utils.widgets import DataTable, StatCard
from ui.dashboard_shell import DashboardShell
from data_paths import data_path


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
        self._csv: dict = {}
        self.teacher_id      = None
        self.assigned_classes = []
        self._resolve_teacher()
        self._build_pages()

    def _csv_rows(self, filename: str) -> list:
        if filename not in self._csv:
            self._csv[filename] = _read_csv(data_path(filename))
        return self._csv[filename]

    def _resolve_teacher(self):
        for r in self._csv_rows('teachers.csv'):
            if len(r) >= 7 and r[1] == self.username:
                self.teacher_id = int(r[0])
                self.assigned_classes = [c for c in r[6].split('|') if c]
                return

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

        # Real stats from CSV
        classes_taught = len(self.assigned_classes)

        student_ids = {
            r[0] for r in self._csv_rows('students.csv')
            if len(r) >= 6 and r[5] in self.assigned_classes
        }
        total_students = len(student_ids)

        att_rows = [r for r in self._csv_rows('attendance.csv')
                    if len(r) >= 5 and r[0] in student_ids]
        att_pct = (
            f"{int(100 * sum(1 for r in att_rows if r[4] == '1') / len(att_rows))}%"
            if att_rows else 'N/A'
        )

        grade_rows = [r for r in self._csv_rows('grades.csv')
                      if len(r) >= 5 and r[0] in student_ids]
        avg_grade = (
            f"{sum(float(r[4]) for r in grade_rows) / len(grade_rows):.1f}%"
            if grade_rows else 'N/A'
        )

        stats_row = QHBoxLayout()
        stats_row.setSpacing(12)
        stats_row.addWidget(StatCard("Classes Taught", str(classes_taught)))
        stats_row.addWidget(StatCard("Total Students", str(total_students)))
        stats_row.addWidget(StatCard("Avg Attendance", att_pct))
        stats_row.addWidget(StatCard("Avg Grade",      avg_grade))
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
