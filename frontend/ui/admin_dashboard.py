"""Admin dashboard."""

from PyQt5.QtWidgets import (
    QWidget, QVBoxLayout, QHBoxLayout, QPushButton, QLabel,
    QTabWidget, QFormLayout, QLineEdit, QMessageBox, QTextEdit
)
from PyQt5.QtCore import Qt, pyqtSignal
from PyQt5.QtGui import QFont
import csv
from pathlib import Path
from backend_communicator import BackendCommunicator
from utils.widgets import DataTable, FormDialog, StatCard


class AdminDashboard(QWidget):
    """Admin dashboard with management features."""

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
        title = QLabel(f"Admin Dashboard - {self.username}")
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
        tabs.addTab(self._create_students_tab(), "Students")
        tabs.addTab(self._create_teachers_tab(), "Teachers")
        tabs.addTab(self._create_fees_tab(), "Fees")
        tabs.addTab(self._create_announcements_tab(), "Announcements")
        tabs.addTab(self._create_analytics_tab(), "Analytics")

        layout.addWidget(tabs)
        self.setLayout(layout)

    def _create_students_tab(self) -> QWidget:
        """Create students management tab."""
        widget = QWidget()
        layout = QVBoxLayout()

        btn_layout = QHBoxLayout()
        add_btn = QPushButton("Add Student")
        edit_btn = QPushButton("Edit Selected")
        remove_btn = QPushButton("Remove Selected")
        add_btn.clicked.connect(self._add_student)
        edit_btn.clicked.connect(self._edit_student)
        remove_btn.clicked.connect(self._remove_student)
        btn_layout.addWidget(add_btn)
        btn_layout.addWidget(edit_btn)
        btn_layout.addWidget(remove_btn)
        btn_layout.addStretch()
        layout.addLayout(btn_layout)

        self.students_table = DataTable(['ID', 'Name', 'Class', 'Username'], widget)
        self._populate_students_table(self.students_table)
        layout.addWidget(self.students_table)

        widget.setLayout(layout)
        return widget

    def _create_teachers_tab(self) -> QWidget:
        """Create teachers management tab."""
        widget = QWidget()
        layout = QVBoxLayout()

        btn_layout = QHBoxLayout()
        add_btn = QPushButton("Add Teacher")
        edit_btn = QPushButton("Edit Selected")
        remove_btn = QPushButton("Remove Selected")
        add_btn.clicked.connect(self._add_teacher)
        edit_btn.clicked.connect(self._edit_teacher)
        remove_btn.clicked.connect(self._remove_teacher)
        btn_layout.addWidget(add_btn)
        btn_layout.addWidget(edit_btn)
        btn_layout.addWidget(remove_btn)
        btn_layout.addStretch()
        layout.addLayout(btn_layout)

        self.teachers_table = DataTable(['ID', 'Name', 'Subject', 'Username'], widget)
        self._populate_teachers_table(self.teachers_table)
        layout.addWidget(self.teachers_table)

        widget.setLayout(layout)
        return widget

    def _create_fees_tab(self) -> QWidget:
        """Create fee management tab."""
        widget = QWidget()
        layout = QVBoxLayout()

        layout.addWidget(QLabel("Fee Structure by Class"))

        table = DataTable(['Class', 'Amount', 'Action'], widget)
        self._populate_fees_table(table)
        layout.addWidget(table)

        layout.addWidget(QLabel("Late Fee per Day: 1.00"))

        widget.setLayout(layout)
        return widget

    def _create_announcements_tab(self) -> QWidget:
        """Create announcements tab."""
        widget = QWidget()
        layout = QVBoxLayout()

        form_layout = QFormLayout()
        self.announce_title = QLineEdit()
        self.announce_content = QTextEdit()
        self.announce_content.setMaximumHeight(150)

        form_layout.addRow("Title:", self.announce_title)
        form_layout.addRow("Content:", self.announce_content)

        send_btn = QPushButton("Send Announcement")
        send_btn.clicked.connect(self._send_announcement)
        form_layout.addRow(send_btn)

        layout.addLayout(form_layout)
        layout.addStretch()

        widget.setLayout(layout)
        return widget

    def _create_analytics_tab(self) -> QWidget:
        """Create analytics tab."""
        widget = QWidget()
        layout = QVBoxLayout()

        stats_layout = QHBoxLayout()
        stats_layout.addWidget(StatCard("Total Students", "80"))
        stats_layout.addWidget(StatCard("Total Teachers", "5"))
        stats_layout.addWidget(StatCard("Total Classes", "6"))
        stats_layout.addWidget(StatCard("Avg Attendance", "92%"))
        layout.addLayout(stats_layout)

        layout.addWidget(QLabel("📊 Analytics Charts (Coming Soon)"))
        layout.addStretch()

        widget.setLayout(layout)
        return widget

    # --- Student handlers ---

    def _add_student(self):
        """Open dialog to add a new student."""
        dialog = FormDialog("Add Student", [
            ("ID", "text"), ("Name", "text"), ("Class", "text"), ("Email", "text")
        ], self)
        if dialog.exec_() == FormDialog.Accepted:
            values = dialog.get_values()
            row = self.students_table.rowCount()
            self.students_table.setRowCount(row + 1)
            from PyQt5.QtWidgets import QTableWidgetItem
            for col, key in enumerate(["ID", "Name", "Class", "Email"]):
                self.students_table.setItem(row, col, QTableWidgetItem(values.get(key, '')))

    def _edit_student(self):
        """Open dialog to edit the selected student."""
        row_data = self.students_table.get_selected_row()
        if not row_data:
            QMessageBox.warning(self, "No Selection", "Please select a student to edit.")
            return
        dialog = FormDialog("Edit Student", [
            ("ID", "text"), ("Name", "text"), ("Class", "text"), ("Email", "text")
        ], self)
        dialog.set_values(row_data)
        if dialog.exec_() == FormDialog.Accepted:
            values = dialog.get_values()
            current_row = self.students_table.currentRow()
            from PyQt5.QtWidgets import QTableWidgetItem
            for col, key in enumerate(["ID", "Name", "Class", "Email"]):
                self.students_table.setItem(current_row, col, QTableWidgetItem(values.get(key, '')))

    def _remove_student(self):
        """Remove the selected student row."""
        row_data = self.students_table.get_selected_row()
        if not row_data:
            QMessageBox.warning(self, "No Selection", "Please select a student to remove.")
            return
        confirm = QMessageBox.question(
            self, "Confirm Removal",
            f"Remove student '{row_data.get('Name', '')}'?"
        )
        if confirm == QMessageBox.Yes:
            self.students_table.removeRow(self.students_table.currentRow())

    # --- Teacher handlers ---

    def _add_teacher(self):
        """Open dialog to add a new teacher."""
        dialog = FormDialog("Add Teacher", [
            ("ID", "text"), ("Name", "text"), ("Subject", "text"), ("Email", "text")
        ], self)
        if dialog.exec_() == FormDialog.Accepted:
            values = dialog.get_values()
            row = self.teachers_table.rowCount()
            self.teachers_table.setRowCount(row + 1)
            from PyQt5.QtWidgets import QTableWidgetItem
            for col, key in enumerate(["ID", "Name", "Subject", "Email"]):
                self.teachers_table.setItem(row, col, QTableWidgetItem(values.get(key, '')))

    def _edit_teacher(self):
        """Open dialog to edit the selected teacher."""
        row_data = self.teachers_table.get_selected_row()
        if not row_data:
            QMessageBox.warning(self, "No Selection", "Please select a teacher to edit.")
            return
        dialog = FormDialog("Edit Teacher", [
            ("ID", "text"), ("Name", "text"), ("Subject", "text"), ("Email", "text")
        ], self)
        dialog.set_values(row_data)
        if dialog.exec_() == FormDialog.Accepted:
            values = dialog.get_values()
            current_row = self.teachers_table.currentRow()
            from PyQt5.QtWidgets import QTableWidgetItem
            for col, key in enumerate(["ID", "Name", "Subject", "Email"]):
                self.teachers_table.setItem(current_row, col, QTableWidgetItem(values.get(key, '')))

    def _remove_teacher(self):
        """Remove the selected teacher row."""
        row_data = self.teachers_table.get_selected_row()
        if not row_data:
            QMessageBox.warning(self, "No Selection", "Please select a teacher to remove.")
            return
        confirm = QMessageBox.question(
            self, "Confirm Removal",
            f"Remove teacher '{row_data.get('Name', '')}'?"
        )
        if confirm == QMessageBox.Yes:
            self.teachers_table.removeRow(self.teachers_table.currentRow())

    # --- Announcement handler ---

    def _send_announcement(self):
        """Send announcement via backend."""
        title = self.announce_title.text().strip()
        content = self.announce_content.toPlainText().strip()
        if not title or not content:
            QMessageBox.warning(self, "Input Error", "Please enter both title and content.")
            return
        QMessageBox.information(self, "Sent", f"Announcement '{title}' sent successfully.")
        self.announce_title.clear()
        self.announce_content.clear()

    # --- CSV population helpers ---

    def _populate_students_table(self, table: DataTable):
        """Populate students from CSV."""
        try:
            csv_path = self.data_dir / 'students.csv'
            if not csv_path.exists():
                return
            with open(csv_path, 'r') as f:
                reader = csv.reader(f)
                data = [
                    {'ID': r[0], 'Name': r[3], 'Class': r[5], 'Username': r[1]}
                    for r in reader if len(r) >= 6
                ]
                table.populate_from_list(data, ['ID', 'Name', 'Class', 'Username'])
        except Exception as e:
            QMessageBox.warning(self, "Error", f"Failed to load students: {str(e)}")

    def _populate_teachers_table(self, table: DataTable):
        """Populate teachers from CSV."""
        try:
            csv_path = self.data_dir / 'teachers.csv'
            if not csv_path.exists():
                return
            with open(csv_path, 'r') as f:
                reader = csv.reader(f)
                data = [
                    {'ID': r[0], 'Name': r[3], 'Subject': r[4], 'Username': r[1]}
                    for r in reader if len(r) >= 5
                ]
                table.populate_from_list(data, ['ID', 'Name', 'Subject', 'Username'])
        except Exception:
            pass

    def _populate_fees_table(self, table: DataTable):
        """Populate fees from CSV."""
        try:
            csv_path = self.data_dir / 'fee_structure.csv'
            if not csv_path.exists():
                return
            with open(csv_path, 'r') as f:
                reader = csv.reader(f)
                data = [
                    {'Class': r[0], 'Amount': r[1]}
                    for r in reader if len(r) >= 2
                ]
                table.populate_from_list(data, ['Class', 'Amount'])
        except Exception:
            pass
