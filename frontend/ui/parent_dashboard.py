"""Parent dashboard — PyDracula style."""

import csv
from pathlib import Path
from PyQt5.QtWidgets import (
    QWidget, QVBoxLayout, QHBoxLayout, QLabel,
    QPushButton, QProgressBar, QTableWidget, QTableWidgetItem, QComboBox
)

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


class ParentDashboard(DashboardShell):
    """Parent dashboard for monitoring child's progress."""

    def __init__(self, backend: BackendCommunicator, username: str):
        super().__init__(username, 'parent')
        self.backend  = backend
        self._csv: dict = {}
        # Resolve parent → child from CSV
        self.child_id    = None
        self.child_name  = 'Unknown'
        self.child_class = ''
        self._resolve_child()
        self._build_pages()

    def _csv_rows(self, filename: str) -> list:
        if filename not in self._csv:
            self._csv[filename] = _read_csv(data_path(filename))
        return self._csv[filename]

    def _resolve_child(self):
        """Find the child linked to this parent username."""
        for r in self._csv_rows('parents.csv'):
            if len(r) >= 5 and r[1] == self.username:
                child_id = int(r[4])
                for s in self._csv_rows('students.csv'):
                    if len(s) >= 6 and int(s[0]) == child_id:
                        self.child_id    = child_id
                        self.child_name  = s[3]
                        self.child_class = s[5]
                return

    # ── Stat calculations ─────────────────────────────────────────

    def _calc_attendance(self) -> str:
        if not self.child_id:
            return 'N/A'
        sid = str(self.child_id)
        rows = [r for r in self._csv_rows('attendance.csv') if len(r) >= 5 and r[0] == sid]
        if not rows:
            return 'N/A'
        present = sum(1 for r in rows if r[4] == '1')
        return f"{int(100 * present / len(rows))}%"

    def _calc_avg_grade(self) -> str:
        if not self.child_id:
            return 'N/A'
        sid = str(self.child_id)
        scores = [float(r[4]) for r in self._csv_rows('grades.csv')
                  if len(r) >= 5 and r[0] == sid]
        return f"{sum(scores)/len(scores):.1f}%" if scores else 'N/A'

    def _calc_fees_due(self) -> str:
        if not self.child_id:
            return 'N/A'
        sid = str(self.child_id)
        for r in self._csv_rows('fees.csv'):
            if len(r) >= 8 and r[1] == sid:
                outstanding = float(r[6]) - float(r[7])
                return f"GHS {outstanding:.2f}"
        return 'N/A'

    def _build_pages(self):
        self.add_nav_button("🏠", "Overview",       self._create_overview_tab())
        self.add_nav_button("✅", "Attendance",      self._create_attendance_tab())
        self.add_nav_button("📝", "Grades",          self._create_grades_tab())
        self.add_nav_button("💰", "Fees",            self._create_fees_tab())
        self.add_nav_button("📢", "Communications",  self._create_communications_tab())

    # ── Pages ─────────────────────────────────────────────────────

    def _create_overview_tab(self) -> QWidget:
        widget = QWidget()
        layout = QVBoxLayout(widget)
        layout.setContentsMargins(20, 20, 20, 20)
        layout.setSpacing(16)

        child_row = QHBoxLayout()
        child_row.setSpacing(8)
        child_row.addWidget(QLabel("Child:"))
        child_combo = QComboBox()
        label = f"{self.child_name}  (Class {self.child_class})" if self.child_id else "No child linked"
        child_combo.addItem(label)
        child_row.addWidget(child_combo)
        child_row.addStretch()
        layout.addLayout(child_row)

        att = self._calc_attendance()
        pct_int = int(att.replace('%', '')) if att != 'N/A' else 0
        status = 'Good' if pct_int >= 75 else 'Low'

        stats_row = QHBoxLayout()
        stats_row.setSpacing(12)
        stats_row.addWidget(StatCard("Attendance", att))
        stats_row.addWidget(StatCard("Avg Grade",  self._calc_avg_grade()))
        stats_row.addWidget(StatCard("Fees Due",   self._calc_fees_due()))
        stats_row.addWidget(StatCard("Status",     status))
        layout.addLayout(stats_row)
        layout.addStretch()
        return widget

    def _create_attendance_tab(self) -> QWidget:
        widget = QWidget()
        layout = QVBoxLayout(widget)
        layout.setContentsMargins(20, 20, 20, 20)
        layout.setSpacing(12)

        att = self._calc_attendance()
        pct = int(att.replace('%', '')) if att != 'N/A' else 0
        bar = QProgressBar()
        bar.setValue(pct)
        bar.setFormat(f"Attendance: {att}")
        layout.addWidget(bar)

        table = DataTable(['Date', 'Status', 'Subject'])
        self._populate_attendance_data(table)
        layout.addWidget(table)
        return widget

    def _create_grades_tab(self) -> QWidget:
        widget = QWidget()
        layout = QVBoxLayout(widget)
        layout.setContentsMargins(20, 20, 20, 20)
        layout.setSpacing(12)

        layout.addWidget(QLabel("Recent Grades"))
        table = DataTable(['Subject', 'Score', 'Term', 'Date'])
        self._populate_grades_data(table)
        layout.addWidget(table)
        return widget

    def _create_fees_tab(self) -> QWidget:
        widget = QWidget()
        layout = QVBoxLayout(widget)
        layout.setContentsMargins(20, 20, 20, 20)
        layout.setSpacing(12)

        summary = QTableWidget()
        summary.setColumnCount(2)
        summary.setHorizontalHeaderLabels(['Description', 'Amount'])
        summary.verticalHeader().setVisible(False)
        summary.horizontalHeader().setStretchLastSection(True)
        summary.setMaximumHeight(130)
        rows = self._get_fee_summary()
        summary.setRowCount(len(rows))
        for i, (desc, amt) in enumerate(rows):
            summary.setItem(i, 0, QTableWidgetItem(desc))
            summary.setItem(i, 1, QTableWidgetItem(amt))
        layout.addWidget(summary)

        pay_btn = QPushButton("Pay Fees Online")
        pay_btn.setFixedWidth(160)
        layout.addWidget(pay_btn)
        layout.addStretch()
        return widget

    def _create_communications_tab(self) -> QWidget:
        widget = QWidget()
        layout = QVBoxLayout(widget)
        layout.setContentsMargins(20, 20, 20, 20)
        layout.setSpacing(12)

        layout.addWidget(QLabel("School Announcements"))
        table = DataTable(['Date', 'Title', 'From', 'Read'])
        self._populate_announcements(table)
        layout.addWidget(table)
        return widget

    # ── CSV helpers ───────────────────────────────────────────────

    def _populate_attendance_data(self, table: DataTable):
        if not self.child_id:
            return
        sid = str(self.child_id)
        data = [
            {'Date': r[3], 'Status': 'Present' if r[4] == '1' else 'Absent', 'Subject': ''}
            for r in self._csv_rows('attendance.csv') if len(r) >= 5 and r[0] == sid
        ][:15]
        table.populate_from_list(data, ['Date', 'Status', 'Subject'])

    def _populate_grades_data(self, table: DataTable):
        if not self.child_id:
            return
        sid = str(self.child_id)
        data = [
            {'Subject': r[2], 'Score': f"{float(r[4]):.1f}", 'Term': r[3], 'Date': ''}
            for r in self._csv_rows('grades.csv') if len(r) >= 5 and r[0] == sid
        ][:15]
        table.populate_from_list(data, ['Subject', 'Score', 'Term', 'Date'])

    def _get_fee_summary(self):
        if not self.child_id:
            return [('No fee data', '')]
        sid = str(self.child_id)
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

    def _populate_announcements(self, table: DataTable):
        data = [
            {'Date': r[3], 'Title': r[1], 'From': r[4], 'Read': 'No'}
            for r in self._csv_rows('announcements.csv') if len(r) >= 5
        ]
        table.populate_from_list(data, ['Date', 'Title', 'From', 'Read'])
