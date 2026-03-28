"""Parent dashboard — PyDracula style."""

import csv
from pathlib import Path
from PyQt5.QtWidgets import (
    QWidget, QVBoxLayout, QHBoxLayout, QLabel,
    QPushButton, QProgressBar, QTableWidget, QTableWidgetItem, QComboBox, QSizePolicy
)
from PyQt5.QtCore import pyqtSignal

from backend_communicator import BackendCommunicator
from utils.widgets import DataTable, StatCard
from ui.dashboard_shell import DashboardShell


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
        self.data_dir = Path(__file__).parent.parent.parent / 'data'
        self._csv: dict = {}
        self._build_pages()

    def _csv_rows(self, filename: str) -> list:
        if filename not in self._csv:
            self._csv[filename] = _read_csv(self.data_dir / filename)
        return self._csv[filename]

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
        child_combo.addItem("John Mensah  (Class 10A)")
        child_row.addWidget(child_combo)
        child_row.addStretch()
        layout.addLayout(child_row)

        stats_row = QHBoxLayout()
        stats_row.setSpacing(12)
        stats_row.addWidget(StatCard("Attendance", "88%"))
        stats_row.addWidget(StatCard("Avg Grade",  "85%"))
        stats_row.addWidget(StatCard("Fees Due",   "GHS 750"))
        stats_row.addWidget(StatCard("Status",     "Good"))
        layout.addLayout(stats_row)
        layout.addStretch()
        return widget

    def _create_attendance_tab(self) -> QWidget:
        widget = QWidget()
        layout = QVBoxLayout(widget)
        layout.setContentsMargins(20, 20, 20, 20)
        layout.setSpacing(12)

        bar = QProgressBar()
        bar.setValue(88)
        bar.setFormat("Monthly Attendance: 88%")
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
        table = DataTable(['Subject', 'Score', 'Grade', 'Date'])
        self._populate_grades_data(table)
        layout.addWidget(table)
        return widget

    def _create_fees_tab(self) -> QWidget:
        widget = QWidget()
        layout = QVBoxLayout(widget)
        layout.setContentsMargins(20, 20, 20, 20)
        layout.setSpacing(12)

        summary = QTableWidget()
        summary.setRowCount(3)
        summary.setColumnCount(2)
        summary.setHorizontalHeaderLabels(['Description', 'Amount'])
        summary.verticalHeader().setVisible(False)
        summary.horizontalHeader().setStretchLastSection(True)
        summary.setMaximumHeight(130)
        for i, (desc, amt) in enumerate([
            ('Annual Fee',   'GHS 1,500.00'),
            ('Paid',         'GHS 750.00'),
            ('Outstanding',  'GHS 750.00'),
        ]):
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
        data = [
            {'Date': r[3], 'Status': 'Present' if r[4] == '1' else 'Absent', 'Subject': ''}
            for r in self._csv_rows('attendance.csv') if len(r) >= 5
        ][:15]
        table.populate_from_list(data, ['Date', 'Status', 'Subject'])

    def _populate_grades_data(self, table: DataTable):
        data = [
            {'Subject': r[2], 'Score': f"{float(r[4]):.1f}", 'Grade': r[3], 'Date': ''}
            for r in self._csv_rows('grades.csv') if len(r) >= 5
        ][:15]
        table.populate_from_list(data, ['Subject', 'Score', 'Grade', 'Date'])

    def _populate_announcements(self, table: DataTable):
        data = [
            {'Date': r[3], 'Title': r[1], 'From': r[4], 'Read': 'No'}
            for r in self._csv_rows('announcements.csv') if len(r) >= 5
        ]
        table.populate_from_list(data, ['Date', 'Title', 'From', 'Read'])
