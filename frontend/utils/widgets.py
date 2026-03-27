"""Common PyQt5 widgets and components."""

from PyQt5.QtWidgets import (
    QWidget, QLabel, QPushButton, QLineEdit, QTableWidget,
    QTableWidgetItem, QVBoxLayout, QHBoxLayout,
    QDialogButtonBox, QDialog, QFormLayout, QTextEdit,
    QSpinBox, QDoubleSpinBox, QCheckBox, QHeaderView
)
from PyQt5.QtCore import Qt
from typing import List, Dict, Tuple, Optional, Any


class DataTable(QTableWidget):
    """Custom table widget with better defaults."""

    def __init__(self, headers: List[str], parent=None):
        super().__init__(parent)
        self.setColumnCount(len(headers))
        self.setHorizontalHeaderLabels(headers)
        self.horizontalHeader().setStyleSheet(
            "QHeaderView::section { background-color: #f0f0f0; padding: 5px; }"
        )
        self.setAlternatingRowColors(True)
        self.horizontalHeader().setSectionResizeMode(QHeaderView.Stretch)

    def populate_from_list(self, data: List[Dict], keys: List[str]):
        """Populate table from list of dicts."""
        self.setRowCount(len(data))
        for row, item in enumerate(data):
            for col, key in enumerate(keys):
                value = str(item.get(key, ''))
                self.setItem(row, col, QTableWidgetItem(value))

    def get_selected_row(self) -> Optional[Dict]:
        """Get selected row as dict."""
        current = self.currentRow()
        if current < 0:
            return None
        return {
            self.horizontalHeaderItem(col).text(): self.item(current, col).text()
            for col in range(self.columnCount())
        }


class FormDialog(QDialog):
    """Generic form dialog with fields."""

    def __init__(self, title: str, fields: List[Tuple[str, str]], parent=None):
        super().__init__(parent)
        self.setWindowTitle(title)
        self.setMinimumWidth(400)
        self.fields = {}
        self._setup_ui(fields)

    def _setup_ui(self, fields: List[Tuple[str, str]]):
        """Setup form fields."""
        layout = QFormLayout()

        for label, field_type in fields:
            if field_type == 'password':
                widget = QLineEdit()
                widget.setEchoMode(QLineEdit.Password)
            elif field_type == 'number':
                widget = QSpinBox()
            elif field_type == 'decimal':
                widget = QDoubleSpinBox()
                widget.setDecimals(2)
            elif field_type == 'multiline':
                widget = QTextEdit()
                widget.setMaximumHeight(100)
            else:
                widget = QLineEdit()

            self.fields[label] = widget
            layout.addRow(label + ':', widget)

        buttons = QDialogButtonBox(QDialogButtonBox.Ok | QDialogButtonBox.Cancel)
        buttons.accepted.connect(self.accept)
        buttons.rejected.connect(self.reject)
        layout.addRow(buttons)

        self.setLayout(layout)

    def get_values(self) -> Dict[str, Any]:
        """Get all form values."""
        result = {}
        for label, widget in self.fields.items():
            if isinstance(widget, QLineEdit):
                result[label] = widget.text()
            elif isinstance(widget, QTextEdit):
                result[label] = widget.toPlainText()
            elif isinstance(widget, (QSpinBox, QDoubleSpinBox)):
                result[label] = widget.value()
            elif isinstance(widget, QCheckBox):
                result[label] = widget.isChecked()
        return result

    def set_values(self, values: Dict[str, Any]):
        """Set form values."""
        for label, value in values.items():
            if label in self.fields:
                widget = self.fields[label]
                if isinstance(widget, QLineEdit):
                    widget.setText(str(value))
                elif isinstance(widget, (QSpinBox, QDoubleSpinBox)):
                    widget.setValue(float(value))


class PasswordLineEdit(QLineEdit):
    """Password field with show/hide toggle."""

    def __init__(self, parent=None):
        super().__init__(parent)
        self.setEchoMode(QLineEdit.Password)
        self.show_button = QPushButton("Show")
        self.show_button.setCheckable(True)
        self.show_button.setMaximumWidth(50)
        self.show_button.clicked.connect(self._toggle_visibility)

    def _toggle_visibility(self):
        if self.show_button.isChecked():
            self.setEchoMode(QLineEdit.Normal)
            self.show_button.setText("Hide")
        else:
            self.setEchoMode(QLineEdit.Password)
            self.show_button.setText("Show")


class StatCard(QWidget):
    """Card displaying a statistic."""

    def __init__(self, title: str, value: str, parent=None):
        super().__init__(parent)
        layout = QVBoxLayout()
        layout.setContentsMargins(10, 10, 10, 10)

        title_label = QLabel(title)
        title_font = title_label.font()
        title_font.setPointSize(9)
        title_label.setFont(title_font)
        title_label.setStyleSheet("color: #666;")

        value_label = QLabel(value)
        value_font = value_label.font()
        value_font.setPointSize(16)
        value_font.setBold(True)
        value_label.setFont(value_font)

        layout.addWidget(title_label)
        layout.addWidget(value_label)
        layout.addStretch()

        self.setLayout(layout)
        self.setStyleSheet("""
            StatCard {
                background-color: white;
                border: 1px solid #e0e0e0;
                border-radius: 5px;
                padding: 10px;
            }
        """)
