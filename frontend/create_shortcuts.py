"""Create a desktop shortcut for the Student Management System (no extra deps)."""

import sys
from pathlib import Path


def create_desktop_shortcut():
    """Write a .bat launcher to the user's Desktop."""
    current_dir = Path(__file__).parent.absolute()
    desktop     = Path.home() / "Desktop"

    if sys.platform == "win32":
        shortcut = desktop / "Student Management System.bat"
        shortcut.write_text(
            f'@echo off\ntitle Student Management System\ncd /d "{current_dir}"\npython main.py\n',
            encoding='utf-8'
        )
        print(f"[SUCCESS] Desktop shortcut created: {shortcut}")
    else:
        # Linux / macOS .desktop file
        shortcut = desktop / "StudentManagementSystem.desktop"
        shortcut.write_text(
            f"[Desktop Entry]\nVersion=1.0\nType=Application\n"
            f"Name=Student Management System\n"
            f"Exec=python \"{current_dir / 'main.py'}\"\n"
            f"Terminal=false\nCategories=Education;\n",
            encoding='utf-8'
        )
        shortcut.chmod(0o755)
        print(f"[SUCCESS] Desktop shortcut created: {shortcut}")


if __name__ == "__main__":
    try:
        create_desktop_shortcut()
    except Exception as e:
        print(f"[ERROR] Could not create shortcut: {e}")
    input("\nPress Enter to continue...")
