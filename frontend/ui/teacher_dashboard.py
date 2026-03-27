"""Teacher dashboard."""

from PyQt5.QtWidgets import (
    QWidget, QVBoxLayout, QHBoxLayout, QPushButton, QLabel,
    QTabWidget, QDateEdit, QTableWidget, QTableWidgetItem, QComboBox,
    QSpinBox, QDoubleSpinBox, QMessageBox, QFormLayout
)
from PyQt5.QtCore import Qt, pyqtSignal, QDate
from PyQt5.QtGui import QFont
import csv
from pathlib import Path
from backend_communicator import BackendCommunicator
from utils.widgets import DataTable, StatCard


class TeacherDashboard(QWidget):
    """Teacher dashboard for attendance and grades."""
    
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
        title = QLabel(f"Teacher Dashboard - {self.username}")
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
        
        # Tabs
        tabs = QTabWidget()
        tabs.addTab(self._create_attendance_tab(), "Mark Attendance")
        tabs.addTab(self._create_grades_tab(), "Add Grades")
        tabs.addTab(self._create_classes_tab(), "My Classes")
        tabs.addTab(self._create_analytics_tab(), "Analytics")
        
        layout.addWidget(tabs)
        self.setLayout(layout)
    
    def _create_attendance_tab(self) -> QWidget:
        """Create attendance marking tab."""
        widget = QWidget()
        layout = QVBoxLayout()
        
        # Date selector
        date_layout = QFormLayout()
        date_edit = QDateEdit()
        date_edit.setDate(QDate.currentDate())
        date_layout.addRow("Date:", date_edit)
        layout.addLayout(date_layout)
        
        # Student list
        table = DataTable(['Student ID', 'Name', 'Status', 'Action'], widget)
        self._populate_attendance_table(table)
        layout.addWidget(table)
        
        # Save button
        save_btn = QPushButton("Save Attendance")
        layout.addWidget(save_btn)
        
        widget.setLayout(layout)
        return widget
    
    def _create_grades_tab(self) -> QWidget:
        """Create grades entry tab."""
        widget = QWidget()
        layout = QVBoxLayout()
        
        # Filters
        filter_layout = QHBoxLayout()
        filter_layout.addWidget(QLabel("Class:"))
        class_combo = QComboBox()
        class_combo.addItems(['10A', '10B', '10C', '11A', '11B', '11C'])
        
        filter_layout.addWidget(class_combo)
        filter_layout.addStretch()
        layout.addLayout(filter_layout)
        
        # Grades table
        table = DataTable(['Student', 'Subject', 'Score', 'Grade', 'Action'], widget)
        self._populate_grades_table(table)
        layout.addWidget(table)
        
        # Save button
        save_btn = QPushButton("Save Grades")
        layout.addWidget(save_btn)
        
        widget.setLayout(layout)
        return widget
    
    def _create_classes_tab(self) -> QWidget:
        """Create my classes tab."""
        widget = QWidget()
        layout = QVBoxLayout()
        
        layout.addWidget(QLabel("Assigned Classes:"))
        
        table = DataTable(['Class', 'Subject', 'Students', 'View Progress'], widget)
        self._populate_classes_table(table)
        layout.addWidget(table)
        
        widget.setLayout(layout)
        return widget
    
    def _create_analytics_tab(self) -> QWidget:
        """Create analytics tab."""
        widget = QWidget()
        layout = QVBoxLayout()
        
        # Stats
        stats_layout = QHBoxLayout()
        stats_layout.addWidget(StatCard("Classes Taught", "2"))
        stats_layout.addWidget(StatCard("Total Students", "50"))
        stats_layout.addWidget(StatCard("Avg Attendance", "88%"))
        stats_layout.addWidget(StatCard("Avg Grade", "85%"))
        layout.addLayout(stats_layout)
        
        layout.addStretch()
        widget.setLayout(layout)
        return widget
    
    def _populate_attendance_table(self, table: DataTable):
        """Populate attendance table from students CSV."""
        try:
            csv_path = self.data_dir / 'students.csv'
            if csv_path.exists():
                with open(csv_path, 'r') as f:
                    data = [
                        {'Student ID': r[0], 'Name': r[3], 'Status': '', 'Action': ''}
                        for r in csv.reader(f) if len(r) >= 4
                    ][:10]
                    table.populate_from_list(data, ['Student ID', 'Name', 'Status', 'Action'])
        except Exception:
            pass

    def _populate_grades_table(self, table: DataTable):
        """Populate grades table."""
        try:
            csv_path = self.data_dir / 'grades.csv'
            if csv_path.exists():
                with open(csv_path, 'r') as f:
                    data = [
                        {'Student': r[1], 'Subject': r[2], 'Score': r[4], 'Grade': '', 'Action': ''}
                        for r in csv.reader(f) if len(r) >= 5
                    ][:10]
                    table.populate_from_list(data, ['Student', 'Subject', 'Score', 'Grade', 'Action'])
        except Exception:
            pass

    def _populate_classes_table(self, table: DataTable):
        """Populate classes table from courses CSV."""
        try:
            csv_path = self.data_dir / 'courses.csv'
            if csv_path.exists():
                with open(csv_path, 'r') as f:
                    data = [
                        {'Class': r[3], 'Subject': r[2], 'Students': '', 'View Progress': ''}
                        for r in csv.reader(f) if len(r) >= 4
                    ]
                    table.populate_from_list(data, ['Class', 'Subject', 'Students', 'View Progress'])
        except Exception:
            pass
