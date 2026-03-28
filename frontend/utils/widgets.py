"""Common PyQt5 widgets and components."""

from PyQt5.QtWidgets import (
    QWidget, QLabel, QPushButton, QLineEdit, QTableWidget,
    QTableWidgetItem, QVBoxLayout, QDialogButtonBox, QDialog, QFormLayout, QTextEdit,
    QSpinBox, QDoubleSpinBox, QCheckBox, QHeaderView, QSizePolicy, QFrame
)
from PyQt5.QtCore import Qt
from typing import List, Dict, Tuple, Optional, Any


class DataTable(QTableWidget):
    """Custom table widget with better defaults."""

    def __init__(self, headers: List[str], parent=None):
        super().__init__(parent)
        self.setColumnCount(len(headers))
        self.setHorizontalHeaderLabels(headers)
        self.setAlternatingRowColors(True)
        self.setShowGrid(True)
        self.setSelectionBehavior(QTableWidget.SelectRows)
        self.setEditTriggers(QTableWidget.NoEditTriggers)
        self.verticalHeader().setVisible(False)
        self.verticalHeader().setDefaultSectionSize(34)
        self.horizontalHeader().setSectionResizeMode(QHeaderView.Stretch)
        self.horizontalHeader().setHighlightSections(False)
        self.setFocusPolicy(Qt.StrongFocus)

    def populate_from_list(self, data: List[Dict], keys: List[str]):
        """Populate table from list of dicts — batched to avoid per-row repaints."""
        self.setSortingEnabled(False)
        self.setUpdatesEnabled(False)
        self.setRowCount(len(data))
        for row, item in enumerate(data):
            for col, key in enumerate(keys):
                cell = QTableWidgetItem(str(item.get(key, '')))
                cell.setFlags(cell.flags() & ~Qt.ItemIsEditable)
                self.setItem(row, col, cell)
        self.setUpdatesEnabled(True)

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
    """Gradient stat card with hover animation — PyOneDark style."""

    def __init__(self, title: str, value: str, accent: str = '#568af2', parent=None):
        super().__init__(parent)
        self.setObjectName("statCard")
        self.setCursor(Qt.PointingHandCursor)

        # Create style strings without format placeholders to avoid KeyError
        self._style_normal = (
            "#statCard {"
            "background: qlineargradient(x1:0,y1:0,x2:1,y2:1,"
            "stop:0 #2c313a, stop:1 #1e2127);"
            "border-radius: 10px;"
            "border: 1px solid #3a3f4b;"
            "}"
        )
        self._style_hovered = (
            "#statCard {"
            "background: qlineargradient(x1:0,y1:0,x2:1,y2:1,"
            "stop:0 #333845, stop:1 #1e2127);"
            "border-radius: 10px;"
            "border: 1px solid #4a5060;"
            "}"
        )
        self.setStyleSheet(self._style_normal)

        layout = QVBoxLayout()
        layout.setContentsMargins(18, 14, 18, 14)
        layout.setSpacing(6)

        title_label = QLabel(title)
        title_label.setObjectName("statCardTitle")

        value_label = QLabel(value)
        value_label.setObjectName("statCardValue")
        value_label.setStyleSheet(
            f"color: {accent}; font-size: 22pt; font-weight: 700; background: transparent;"
        )

        bar = QFrame()
        bar.setFixedHeight(3)
        bar.setStyleSheet(
            f"background: qlineargradient(x1:0,y1:0,x2:1,y2:0,"
            f" stop:0 {accent}, stop:1 transparent); border-radius: 2px;"
        )

        layout.addWidget(title_label)
        layout.addWidget(value_label)
        layout.addStretch()
        layout.addWidget(bar)
        self.setLayout(layout)
        self.setMinimumHeight(100)
        self.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Fixed)

    def enterEvent(self, event):
        self.setStyleSheet(self._style_hovered)
        super().enterEvent(event)

    def leaveEvent(self, event):
        self.setStyleSheet(self._style_normal)
        super().leaveEvent(event)
