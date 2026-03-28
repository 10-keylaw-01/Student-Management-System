"""Student dashboard — PyDracula style."""

import csv
from pathlib import Path
from PyQt5.QtWidgets import (
    QWidget, QVBoxLayout, QHBoxLayout, QLabel,
    QProgressBar, QTableWidget, QTableWidgetItem, QSizePolicy
)

from backend_communicator import BackendCommunicator
from utils.widgets import DataTable, StatCard
from ui.dashboard_shell import DashboardShell
from data_paths import data_path


def _read_csv(path: Path) -> list:
    """Read a CSV file and return rows as lists. Returns [] on error."""
    try:
        with open(path, 'r', encoding='utf-8', errors='replace') as f:
            return list(csv.reader(f))
    except Exception:
        return []


class StudentDashboard(DashboardShell):
    """Student dashboard — shows data filtered for the logged-in student."""

    def __init__(self, backend: BackendCommunicator, username: str):
        super().__init__(username, 'student')
        self.backend       = backend
        self.data_dir      = None  # unused — paths resolved via data_path()
        self.student_id    = None
        self.student_name  = username
        self.student_class = ''
        # CSV cache: populated lazily, lives for the lifetime of this dashboard
        self._csv: dict[str, list] = {}
        self._resolve_student()
        self._build_pages()

    # ── CSV cache ─────────────────────────────────────────────────

    def _csv_rows(self, filename: str) -> list:
        """Return cached rows for a CSV file."""
        if filename not in self._csv:
            self._csv[filename] = _read_csv(data_path(filename))
        return self._csv[filename]

    # ── Student lookup ────────────────────────────────────────────

    def _resolve_student(self):
        for r in self._csv_rows('students.csv'):
            if len(r) >= 7 and r[1] == self.username:
                self.student_id    = int(r[0])
                self.student_name  = r[3]
                self.student_class = r[5]
                return

    # ── Stats helpers ─────────────────────────────────────────────

    def _calc_attendance(self) -> str:
        if not self.student_id:
            return 'N/A'
        sid = str(self.student_id)
        rows = [r for r in self._csv_rows('attendance.csv') if len(r) >= 5 and r[0] == sid]
        if not rows:
            return 'N/A'
        present = sum(1 for r in rows if r[4] == '1')
        return f"{int(100 * present / len(rows))}%"

    def _calc_avg_grade(self) -> str:
        if not self.student_id:
            return 'N/A'
        sid = str(self.student_id)
        scores = [float(r[4]) for r in self._csv_rows('grades.csv')
                  if len(r) >= 5 and r[0] == sid]
        return f"{sum(scores)/len(scores):.1f}%" if scores else 'N/A'

    def _calc_fee_status(self) -> str:
        if not self.student_id:
            return 'N/A'
        sid = str(self.student_id)
        for r in self._csv_rows('fees.csv'):
            if len(r) >= 8 and r[1] == sid:
                amount, paid = float(r[6]), float(r[7])
                return f"{int(100 * paid / amount)}%" if amount else 'N/A'
        return 'N/A'

    def _count_assignments(self) -> str:
        if not self.student_class:
            return 'N/A'
        count = sum(1 for r in self._csv_rows('assignments.csv')
                    if len(r) >= 7 and r[6] == self.student_class)
        return str(count)

    # ── Pages ─────────────────────────────────────────────────────

    def _build_pages(self):
        self.add_nav_button("🏠", "Overview",     self._create_overview_tab())
        self.add_nav_button("✅", "Attendance",    self._create_attendance_tab())
        self.add_nav_button("📝", "Grades",        self._create_grades_tab())
        self.add_nav_button("💰", "Fees",          self._create_fees_tab())
        self.add_nav_button("📚", "Assignments",   self._create_assignments_tab())
        self.add_nav_button("🗓", "Timetable",     self._create_timetable_tab())
        self.add_nav_button("🔔", "Notifications", self._create_notifications_tab())

    def _create_overview_tab(self) -> QWidget:
        widget = QWidget()
        layout = QVBoxLayout(widget)
        layout.setContentsMargins(20, 20, 20, 20)
        layout.setSpacing(16)

        name_lbl = QLabel(f"Welcome, {self.student_name}   —   Class {self.student_class}")
        name_lbl.setObjectName("welcomeLabel")
        layout.addWidget(name_lbl)

        stats_row = QHBoxLayout()
        stats_row.setSpacing(12)
        stats_row.addWidget(StatCard("Attendance",  self._calc_attendance()))
        stats_row.addWidget(StatCard("Avg Grade",   self._calc_avg_grade()))
        stats_row.addWidget(StatCard("Fees Paid",   self._calc_fee_status()))
        stats_row.addWidget(StatCard("Assignments", self._count_assignments()))
        layout.addLayout(stats_row)
        layout.addStretch()
        return widget

    def _create_attendance_tab(self) -> QWidget:
        widget = QWidget()
        layout = QVBoxLayout(widget)
        layout.setContentsMargins(20, 20, 20, 20)
        layout.setSpacing(12)

        pct_str = self._calc_attendance().replace('%', '')
        pct = int(pct_str) if pct_str.isdigit() else 0
        bar = QProgressBar()
        bar.setValue(pct)
        bar.setFormat(f"Attendance: {pct}%")
        layout.addWidget(bar)

        table = DataTable(['Date', 'Status'])
        self._fill_attendance(table)
        table.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
        layout.addWidget(table, 1)
        return widget

    def _create_grades_tab(self) -> QWidget:
        widget = QWidget()
        layout = QVBoxLayout(widget)
        layout.setContentsMargins(20, 20, 20, 20)
        table = DataTable(['Subject', 'Term', 'Score', 'Total'])
        self._fill_grades(table)
        table.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
        layout.addWidget(table, 1)
        return widget

    def _create_fees_tab(self) -> QWidget:
        widget = QWidget()
        layout = QVBoxLayout(widget)
        layout.setContentsMargins(20, 20, 20, 20)
        layout.setSpacing(12)

        layout.addWidget(QLabel("Fee Summary"))
        summary = QTableWidget()
        summary.setColumnCount(2)
        summary.setHorizontalHeaderLabels(['Description', 'Amount'])
        summary.verticalHeader().setVisible(False)
        summary.horizontalHeader().setStretchLastSection(True)
        summary.setMaximumHeight(180)
        fee_rows = self._get_fee_summary()
        summary.setRowCount(len(fee_rows))
        for i, (desc, amt) in enumerate(fee_rows):
            summary.setItem(i, 0, QTableWidgetItem(desc))
            summary.setItem(i, 1, QTableWidgetItem(amt))
        layout.addWidget(summary)

        layout.addWidget(QLabel("Fee Structure for Your Class"))
        struct_table = DataTable(['Class', 'Termly Fee (GHS)'])
        self._fill_fee_structure(struct_table)
        struct_table.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
        layout.addWidget(struct_table, 1)
        return widget

    def _create_assignments_tab(self) -> QWidget:
        widget = QWidget()
        layout = QVBoxLayout(widget)
        layout.setContentsMargins(20, 20, 20, 20)
        table = DataTable(['Assignment', 'Subject', 'Due Date'])
        self._fill_assignments(table)
        table.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
        layout.addWidget(table, 1)
        return widget

    def _create_timetable_tab(self) -> QWidget:
        widget = QWidget()
        layout = QVBoxLayout(widget)
        layout.setContentsMargins(20, 20, 20, 20)
        layout.setSpacing(12)
        layout.addWidget(QLabel(f"Timetable — Class {self.student_class}"))
        table = DataTable(['Day', 'Start', 'End', 'Subject', 'Teacher'])
        self._fill_timetable(table)
        table.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
        layout.addWidget(table, 1)
        return widget

    def _create_notifications_tab(self) -> QWidget:
        widget = QWidget()
        layout = QVBoxLayout(widget)
        layout.setContentsMargins(20, 20, 20, 20)
        table = DataTable(['Date', 'Category', 'Message'])
        self._fill_notifications(table)
        table.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
        layout.addWidget(table, 1)
        return widget

    # ── Table fillers (use cache) ─────────────────────────────────

    def _fill_attendance(self, table: DataTable):
        if not self.student_id:
            return
        sid = str(self.student_id)
        data = [
            {'Date': r[3], 'Status': 'Present' if r[4] == '1' else 'Absent'}
            for r in self._csv_rows('attendance.csv')
            if len(r) >= 5 and r[0] == sid
        ]
        table.populate_from_list(data, ['Date', 'Status'])

    def _fill_grades(self, table: DataTable):
        if not self.student_id:
            return
        sid = str(self.student_id)
        data = [
            {'Subject': r[2], 'Term': r[3],
             'Score': f"{float(r[4]):.1f}", 'Total': f"{float(r[5]):.0f}"}
            for r in self._csv_rows('grades.csv')
            if len(r) >= 6 and r[0] == sid
        ]
        table.populate_from_list(data, ['Subject', 'Term', 'Score', 'Total'])

    def _get_fee_summary(self):
        if not self.student_id:
            return [('No fee data', '')]
        sid = str(self.student_id)
        for r in self._csv_rows('fees.csv'):
            if len(r) >= 9 and r[1] == sid:
                amount, paid = float(r[6]), float(r[7])
                return [
                    ('Termly Fee',  f"GHS {amount:.2f}"),
                    ('Paid',        f"GHS {paid:.2f}"),
                    ('Outstanding', f"GHS {amount - paid:.2f}"),
                    ('Status',      'Settled' if int(r[8]) else 'Outstanding'),
                    ('Due Date',    r[4]),
                ]
        return [('No fee data', '')]

    def _fill_fee_structure(self, table: DataTable):
        if not self.student_class:
            return
        data = [
            {'Class': r[0], 'Termly Fee (GHS)': f"{float(r[1]):.2f}"}
            for r in self._csv_rows('fee_structure.csv')
            if len(r) >= 2 and r[0] == self.student_class
        ]
        table.populate_from_list(data, ['Class', 'Termly Fee (GHS)'])

    def _fill_assignments(self, table: DataTable):
        if not self.student_class:
            return
        data = [
            {'Assignment': r[2], 'Subject': r[3], 'Due Date': r[4]}
            for r in self._csv_rows('assignments.csv')
            if len(r) >= 7 and r[6] == self.student_class
        ]
        table.populate_from_list(data, ['Assignment', 'Subject', 'Due Date'])

    def _fill_timetable(self, table: DataTable):
        if not self.student_class:
            return
        data = [
            {'Day': r[1], 'Start': r[2], 'End': r[3], 'Subject': r[4], 'Teacher': r[5]}
            for r in self._csv_rows('timetable.csv')
            if len(r) >= 6 and r[0] == self.student_class
        ]
        table.populate_from_list(data, ['Day', 'Start', 'End', 'Subject', 'Teacher'])

    def _fill_notifications(self, table: DataTable):
        if not self.student_id:
            return
        sid = str(self.student_id)
        data = [
            {'Date': r[3], 'Category': r[4], 'Message': r[2]}
            for r in self._csv_rows('notifications.csv')
            if len(r) >= 5 and (r[1] == sid or r[1] == '-1')
        ]
        table.populate_from_list(data, ['Date', 'Category', 'Message'])
