"""Parent dashboard."""

from PyQt5.QtWidgets import (
    QWidget, QVBoxLayout, QHBoxLayout, QPushButton, QLabel,
    QTabWidget, QTableWidget, QTableWidgetItem, QProgressBar, QComboBox
)
from PyQt5.QtCore import Qt, pyqtSignal
from PyQt5.QtGui import QFont
import csv
from pathlib import Path
from backend_communicator import BackendCommunicator
from utils.widgets import DataTable, StatCard


class ParentDashboard(QWidget):
    """Parent dashboard for monitoring child's progress."""
    
    logout = pyqtSignal()
    
    def __init__(self, backend: BackendCommunicator, username: str):
        super().__init__()
        self.backend = backend
        self.username = username
        self.data_dir = Path(__file__).parent.parent.parent / 'data'
        self._setup_ui()
    
    def _setup_ui(self):
        """Setup dashboard UI."""
        layout = QVBoxLayout()
        
        # Header
        header_layout = QHBoxLayout()
        title = QLabel(f"Parent Dashboard - {self.username}")
        title_font = title.font()
        title_font.setPointSize(14)
        title_font.setBold(True)
        title.setFont(title_font)
        
        logout_btn = QPushButton("Logout")
        logout_btn.clicked.connect(self.logout.emit)
        
        header_layout.addWidget(title)
        header_layout.addStretch()
        header_layout.addWidget(logout_btn)
        layout.addLayout(header_layout)
        
        # Child selector
        child_layout = QHBoxLayout()
        child_layout.addWidget(QLabel("Child:"))
        child_combo = QComboBox()
        child_combo.addItem("John Mensah (Class 10A)")
        child_layout.addWidget(child_combo)
        child_layout.addStretch()
        layout.addLayout(child_layout)
        
        # Stats
        stats_layout = QHBoxLayout()
        stats_layout.addWidget(StatCard("Attendance", "88%"))
        stats_layout.addWidget(StatCard("Avg Grade", "85%"))
        stats_layout.addWidget(StatCard("Fees Due", "GHS 750.00"))
        stats_layout.addWidget(StatCard("Status", "Good"))
        layout.addLayout(stats_layout)
        
        # Tabs
        tabs = QTabWidget()
        tabs.addTab(self._create_attendance_tab(), "Attendance")
        tabs.addTab(self._create_grades_tab(), "Grades")
        tabs.addTab(self._create_fees_tab(), "Fees")
        tabs.addTab(self._create_communications_tab(), "Communications")
        
        layout.addWidget(tabs)
        self.setLayout(layout)
    
    def _create_attendance_tab(self) -> QWidget:
        """Create attendance view tab."""
        widget = QWidget()
        layout = QVBoxLayout()
        
        # Progress
        progress = QProgressBar()
        progress.setValue(88)
        progress.setFormat("Monthly Attendance: 88%")
        layout.addWidget(progress)
        
        # Details
        table = DataTable(['Date', 'Status', 'Subject'], widget)
        self._populate_attendance_data(table)
        layout.addWidget(table)
        
        widget.setLayout(layout)
        return widget
    
    def _create_grades_tab(self) -> QWidget:
        """Create grades view tab."""
        widget = QWidget()
        layout = QVBoxLayout()
        
        layout.addWidget(QLabel("Recent Grades"))
        
        table = DataTable(['Subject', 'Score', 'Grade', 'Date'], widget)
        self._populate_grades_data(table)
        layout.addWidget(table)
        
        widget.setLayout(layout)
        return widget
    
    def _create_fees_tab(self) -> QWidget:
        """Create fees view tab."""
        widget = QWidget()
        layout = QVBoxLayout()
        
        # Bills summary
        summary_table = QTableWidget()
        summary_table.setRowCount(3)
        summary_table.setColumnCount(2)
        summary_table.setHorizontalHeaderLabels(['Description', 'Amount'])
        
        summary_data = [
            ('Annual Fee', 'GHS 1,500.00'),
            ('Paid', 'GHS 750.00'),
            ('Outstanding', 'GHS 750.00')
        ]
        
        for row, (desc, amount) in enumerate(summary_data):
            summary_table.setItem(row, 0, QTableWidgetItem(desc))
            summary_table.setItem(row, 1, QTableWidgetItem(amount))
        
        layout.addWidget(summary_table)
        
        # Payment button
        pay_btn = QPushButton("Pay Fees Online")
        pay_btn.setMaximumWidth(200)
        layout.addWidget(pay_btn)
        
        layout.addStretch()
        widget.setLayout(layout)
        return widget
    
    def _create_communications_tab(self) -> QWidget:
        """Create communications tab."""
        widget = QWidget()
        layout = QVBoxLayout()
        
        layout.addWidget(QLabel("School Announcements"))
        
        table = DataTable(['Date', 'Title', 'From', 'Read'], widget)
        self._populate_announcements(table)
        layout.addWidget(table)
        
        widget.setLayout(layout)
        return widget
    
    def _populate_attendance_data(self, table: DataTable):
        """Populate attendance data."""
        try:
            csv_path = self.data_dir / 'attendance.csv'
            if csv_path.exists():
                with open(csv_path, 'r') as f:
                    data = [
                        {'Date': r[3], 'Status': 'Present' if r[4] == '1' else 'Absent', 'Subject': ''}
                        for r in csv.reader(f) if len(r) >= 5
                    ][:10]
                    table.populate_from_list(data, ['Date', 'Status', 'Subject'])
        except Exception:
            pass

    def _populate_grades_data(self, table: DataTable):
        """Populate grades data."""
        try:
            csv_path = self.data_dir / 'grades.csv'
            if csv_path.exists():
                with open(csv_path, 'r') as f:
                    data = [
                        {'Subject': r[2], 'Score': r[4], 'Grade': r[3], 'Date': ''}
                        for r in csv.reader(f) if len(r) >= 5
                    ][:10]
                    table.populate_from_list(data, ['Subject', 'Score', 'Grade', 'Date'])
        except Exception:
            pass

    def _populate_announcements(self, table: DataTable):
        """Populate announcements."""
        try:
            csv_path = self.data_dir / 'announcements.csv'
            if csv_path.exists():
                with open(csv_path, 'r') as f:
                    data = [
                        {'Date': r[3], 'Title': r[1], 'From': r[4], 'Read': 'No'}
                        for r in csv.reader(f) if len(r) >= 5
                    ][:10]
                    table.populate_from_list(data, ['Date', 'Title', 'From', 'Read'])
        except Exception:
            pass
