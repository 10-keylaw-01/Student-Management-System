"""Resource manager for handling application assets like logos and icons."""

import os
from pathlib import Path
from PyQt5.QtGui import QIcon, QPixmap
from PyQt5.QtCore import Qt

class ResourceManager:
    """Manages application resources like logos, icons, and images."""
    
    def __init__(self):
        self.base_path = Path(__file__).parent / "resources"
        self.icons_path = self.base_path / "icons"
        
        # Ensure directories exist
        self.base_path.mkdir(exist_ok=True)
        self.icons_path.mkdir(exist_ok=True)
    
    def get_app_icon(self) -> QIcon:
        """Get the main application icon."""
        icon_paths = [
            self.icons_path / "app_icon.ico",
            self.icons_path / "app_icon.png",
            self.icons_path / "logo.png"
        ]
        
        for path in icon_paths:
            if path.exists():
                return QIcon(str(path))
        
        # Return default icon if no custom icon found
        return self._create_default_icon()
    
    def get_logo_pixmap(self, size: tuple = None) -> QPixmap:
        """Get the application logo as QPixmap."""
        logo_paths = [
            self.icons_path / "logo.png",
            self.icons_path / "app_icon.png"
        ]
        
        for path in logo_paths:
            if path.exists():
                pixmap = QPixmap(str(path))
                if size:
                    pixmap = pixmap.scaled(size[0], size[1], Qt.KeepAspectRatio, Qt.SmoothTransformation)
                return pixmap
        
        # Return default logo if no custom logo found
        return self._create_default_logo(size)
    
    def _create_default_icon(self) -> QIcon:
        """Create a default icon programmatically."""
        pixmap = QPixmap(64, 64)
        pixmap.fill(Qt.transparent)
        
        from PyQt5.QtGui import QPainter, QBrush, QColor
        painter = QPainter(pixmap)
        painter.setRenderHint(QPainter.Antialiasing)
        
        # Draw a simple school/education icon
        painter.setBrush(QBrush(QColor(100, 65, 200)))  # Purple background
        painter.drawRoundedRect(8, 8, 48, 48, 8, 8)
        
        painter.setBrush(QBrush(QColor(255, 255, 255)))  # White text
        painter.drawText(16, 35, "SMS")
        
        painter.end()
        return QIcon(pixmap)
    
    def _create_default_logo(self, size: tuple = None) -> QPixmap:
        """Create a default logo programmatically."""
        if not size:
            size = (200, 80)
        
        pixmap = QPixmap(size[0], size[1])
        pixmap.fill(Qt.transparent)
        
        from PyQt5.QtGui import QPainter, QBrush, QColor, QFont
        painter = QPainter(pixmap)
        painter.setRenderHint(QPainter.Antialiasing)
        
        # Draw logo background
        painter.setBrush(QBrush(QColor(100, 65, 200, 200)))
        painter.drawRoundedRect(10, 10, size[0]-20, size[1]-20, 10, 10)
        
        # Draw text
        painter.setPen(QColor(255, 255, 255))
        font = QFont("Segoe UI", 16, QFont.Bold)
        painter.setFont(font)
        painter.drawText(20, size[1]//2 + 5, "Student Management")
        
        painter.end()
        return pixmap
    
    def save_default_assets(self):
        """Save default assets to disk for customization."""
        # Save default icon
        icon_pixmap = self._create_default_icon().pixmap(64, 64)
        icon_pixmap.save(str(self.icons_path / "default_icon.png"))
        
        # Save default logo
        logo_pixmap = self._create_default_logo((400, 160))
        logo_pixmap.save(str(self.icons_path / "default_logo.png"))
        
        print(f"Default assets saved to: {self.icons_path}")
        print("You can replace these with your custom logo and icon files.")

# Global instance
resource_manager = ResourceManager()