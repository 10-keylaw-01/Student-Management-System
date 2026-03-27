"""Student dashboard."""

from PyQt5.QtWidgets import (
    QWidget, QVBoxLayout, QHBoxLayout, QPushButton, QLabel,
    QTabWidget, QTableWidget, QTableWidgetItem, QProgressBar, QMessageBox
)
from PyQt5.QtCore import Qt, pyqtSignal
from PyQt5.QtGui import QFont
import csv
from pathlib import Path
from backend_communicator import BackendCommunicator
from utils.widgets import DataTable, StatCard


class StudentDashboard(QWidget):
    """Student dashboard — shows data filtered for the logged-in student."""

    logout = pyqtSignal()

    def __init__(self, backend: BackendCommunicator, username: str):
        super().__init__()
        self.backend  = backend
        self.username = username
        self.data_dir = Path(__file__).parent.parent.parent / 'data'

        # Resolve student record from CSV
        self.student_id   = None
        self.student_name = username
        self.student_class = ''
        self._resolve_student()

        self._setup_ui()

    # ── Student lookup ────────────────────────────────────────────────────────

    def _resolve_student(self):
        """Find student ID, name and class from students.csv by username."""
        try:
            with open(self.data_dir / 'students.csv', 'r') as f:
                for r in csv.reader(f):
                    if len(r) >= 7 and r[1] == self.username:
                        self.student_id    = int(r[0])
                        self.student_name  = r[3]
                        self.student_class = r[5]
                        return
        except Exception:
            pass

    # ── Stats helpers ─────────────────────────────────────────────────────────

    def _calc_attendance(self) -> str:
        if not self.student_id:
            return 'N/A'
        try:
            total = present = 0
            with open(self.data_dir / 'attendance.csv', 'r') as f:
                for r in csv.reader(f):
                    if len(r) >= 5 and r[0] == str(self.student_id):
                        total += 1
                        if r[4] == '1':
                            present += 1
            return f"{int(100 * present / total)}%" if total else 'N/A'
        except Exception:
            return 'N/A'

    def _calc_avg_grade(self) -> str:
        if not self.student_id:
            return 'N/A'
        try:
            scores = []
            with open(self.data_dir / 'grades.csv', 'r') as f:
                for r in csv.reader(f):
                    if len(r) >= 5 and r[0] == str(self.student_id):
                        scores.append(float(r[4]))
            return f"{sum(scores)/len(scores):.1f}%" if scores else 'N/A'
        except Exception:
            return 'N/A'

    def _calc_fee_status(self) -> str:
        if not self.student_id:
            return 'N/A'
        try:
            with open(self.data_dir / 'fees.csv', 'r') as f:
                for r in csv.reader(f):
                    if len(r) >= 8 and r[1] == str(self.student_id):
                        amount = float(r[6])
                        paid   = float(r[7])
                        if amount > 0:
                            return f"{int(100 * paid / amount)}%"
            return 'N/A'
        except Exception:
            return 'N/A'

    def _count_pending_assignments(self) -> str:
        if not self.student_class:
            return 'N/A'
        try:
            count = 0
            with open(self.data_dir / 'assignments.csv', 'r') as f:
                for r in csv.reader(f):
                    if len(r) >= 7 and r[6] == self.student_class:
                        count += 1
            return f"{count} Assignments"
        except Exception:
            return 'N/A'

    # ── UI setup ──────────────────────────────────────────────────────────────

    def _setup_ui(self):
        layout = QVBoxLayout()

        # Header
        header = QHBoxLayout()
        title  = QLabel(f"Student Dashboard — {self.student_name} ({self.student_class})")
        font   = title.font()
        font.setPointSize(14)
        font.setBold(True)
        title.setFont(font)
        logout_btn = QPushButton("Logout")
        logout_btn.clicked.connect(self.logout.emit)
        header.addWidget(title)
        header.addStretch()
        header.addWidget(logout_btn)
        layout.addLayout(header)

        # Stats
        stats = QHBoxLayout()
        stats.addWidget(StatCard("Attendance",  self._calc_attendance()))
        stats.addWidget(StatCard("Avg Grade",   self._calc_avg_grade()))
        stats.addWidget(StatCard("Fees Paid",   self._calc_fee_status()))
        stats.addWidget(StatCard("Assignments", self._count_pending_assignments()))
        layout.addLayout(stats)

        # Tabs
        tabs = QTabWidget()
        tabs.addTab(self._create_attendance_tab(),  "Attendance")
        tabs.addTab(self._create_grades_tab(),      "Grades")
        tabs.addTab(self._create_fees_tab(),        "Fees")
        tabs.addTab(self._create_assignments_tab(), "Assignments")
        tabs.addTab(self._create_timetable_tab(),   "Timetable")
        tabs.addTab(self._create_notifications_tab(), "Notifications")

        layout.addWidget(tabs)
        self.setLayout(layout)

    # ── Tabs ──────────────────────────────────────────────────────────────────

    def _create_attendance_tab(self) -> QWidget:
        widget = QWidget()
        layout = QVBoxLayout()

        pct_str = self._calc_attendance().replace('%', '')
        try:
            pct = int(pct_str)
        except ValueError:
            pct = 0
        bar = QProgressBar()
        bar.setValue(pct)
        bar.setFormat(f"Attendance: {pct}%")
        layout.addWidget(bar)

        table = DataTable(['Date', 'Status'], widget)
        self._populate_attendance_table(table)
        layout.addWidget(table)
        widget.setLayout(layout)
        return widget

    def _create_grades_tab(self) -> QWidget:
        widget = QWidget()
        layout = QVBoxLayout()
        table  = DataTable(['Subject', 'Term', 'Score', 'Total'], widget)
        self._populate_grades_table(table)
        layout.addWidget(table)
        widget.setLayout(layout)
        return widget

    def _create_fees_tab(self) -> QWidget:
        widget = QWidget()
        layout = QVBoxLayout()

        layout.addWidget(QLabel("Fee Summary"))
        summary = QTableWidget()
        summary.setColumnCount(2)
        summary.setHorizontalHeaderLabels(['Description', 'Amount'])

        fee_rows = self._get_fee_summary()
        summary.setRowCount(len(fee_rows))
        for i, (desc, amt) in enumerate(fee_rows):
            summary.setItem(i, 0, QTableWidgetItem(desc))
            summary.setItem(i, 1, QTableWidgetItem(amt))
        layout.addWidget(summary)

        layout.addWidget(QLabel("Fee Structure for Your Class"))
        struct_table = DataTable(['Class', 'Termly Fee'], widget)
        self._populate_fee_structure(struct_table)
        layout.addWidget(struct_table)

        widget.setLayout(layout)
        return widget

    def _create_assignments_tab(self) -> QWidget:
        widget = QWidget()
        layout = QVBoxLayout()
        table  = DataTable(['Assignment', 'Subject', 'Due Date'], widget)
        self._populate_assignments_table(table)
        layout.addWidget(table)
        widget.setLayout(layout)
        return widget

    def _create_timetable_tab(self) -> QWidget:
        widget = QWidget()
        layout = QVBoxLayout()
        layout.addWidget(QLabel(f"Timetable for Class {self.student_class}"))
        table  = DataTable(['Day', 'Start', 'End', 'Subject', 'Teacher'], widget)
        self._populate_timetable(table)
        layout.addWidget(table)
        widget.setLayout(layout)
        return widget

    def _create_notifications_tab(self) -> QWidget:
        widget = QWidget()
        layout = QVBoxLayout()
        table  = DataTable(['Date', 'Category', 'Message'], widget)
        self._populate_notifications(table)
        layout.addWidget(table)
        widget.setLayout(layout)
        return widget

    # ── Data population ───────────────────────────────────────────────────────

    def _populate_attendance_table(self, table: DataTable):
        if not self.student_id:
            return
        try:
            with open(self.data_dir / 'attendance.csv', 'r') as f:
                data = [
                    {'Date': r[3], 'Status': 'Present' if r[4] == '1' else 'Absent'}
                    for r in csv.reader(f)
                    if len(r) >= 5 and r[0] == str(self.student_id)
                ]
                table.populate_from_list(data, ['Date', 'Status'])
        except Exception:
            pass

    def _populate_grades_table(self, table: DataTable):
        if not self.student_id:
            return
        try:
            with open(self.data_dir / 'grades.csv', 'r') as f:
                data = [
                    {'Subject': r[2], 'Term': r[3], 'Score': r[4], 'Total': r[5]}
                    for r in csv.reader(f)
                    if len(r) >= 6 and r[0] == str(self.student_id)
                ]
                table.populate_from_list(data, ['Subject', 'Term', 'Score', 'Total'])
        except Exception:
            pass

    def _get_fee_summary(self):
        if not self.student_id:
            return [('No fee data', '')]
        try:
            with open(self.data_dir / 'fees.csv', 'r') as f:
                for r in csv.reader(f):
                    if len(r) >= 9 and r[1] == str(self.student_id):
                        amount    = float(r[6])
                        paid      = float(r[7])
                        outstanding = amount - paid
                        settled   = 'Settled' if int(r[8]) else 'Outstanding'
                        return [
                            ('Termly Fee',   f"GHS {amount:.2f}"),
                            ('Paid',         f"GHS {paid:.2f}"),
                            ('Outstanding',  f"GHS {outstanding:.2f}"),
                            ('Status',       settled),
                            ('Due Date',     r[4]),
                        ]
        except Exception:
            pass
        return [('No fee data', '')]

    def _populate_fee_structure(self, table: DataTable):
        if not self.student_class:
            return
        try:
            with open(self.data_dir / 'fee_structure.csv', 'r') as f:
                data = [
                    {'Class': r[0], 'Termly Fee': f"GHS {float(r[1]):.2f}"}
                    for r in csv.reader(f)
                    if len(r) >= 2 and r[0] == self.student_class
                ]
                table.populate_from_list(data, ['Class', 'Termly Fee'])
        except Exception:
            pass

    def _populate_assignments_table(self, table: DataTable):
        if not self.student_class:
            return
        try:
            with open(self.data_dir / 'assignments.csv', 'r') as f:
                data = [
                    {'Assignment': r[2], 'Subject': r[3], 'Due Date': r[4]}
                    for r in csv.reader(f)
                    if len(r) >= 7 and r[6] == self.student_class
                ]
                table.populate_from_list(data, ['Assignment', 'Subject', 'Due Date'])
        except Exception:
            pass

    def _populate_timetable(self, table: DataTable):
        if not self.student_class:
            return
        try:
            with open(self.data_dir / 'timetable.csv', 'r') as f:
                data = [
                    {'Day': r[1], 'Start': r[2], 'End': r[3], 'Subject': r[4], 'Teacher': r[5]}
                    for r in csv.reader(f)
                    if len(r) >= 6 and r[0] == self.student_class
                ]
                table.populate_from_list(data, ['Day', 'Start', 'End', 'Subject', 'Teacher'])
        except Exception:
            pass

    def _populate_notifications(self, table: DataTable):
        if not self.student_id:
            return
        try:
            with open(self.data_dir / 'notifications.csv', 'r') as f:
                data = [
                    {'Date': r[3], 'Category': r[4], 'Message': r[2]}
                    for r in csv.reader(f)
                    if len(r) >= 5 and (r[1] == str(self.student_id) or r[1] == '-1')
                ]
                table.populate_from_list(data, ['Date', 'Category', 'Message'])
        except Exception:
            pass
