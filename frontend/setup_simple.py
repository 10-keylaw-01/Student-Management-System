"""Setup script for Student Management System desktop application."""

import subprocess
import sys
from pathlib import Path


def install_requirements():
    """Install required Python packages using pre-built wheels only."""
    requirements_file = Path(__file__).parent / "requirements.txt"

    if not requirements_file.exists():
        print("[ERROR] requirements.txt not found")
        return False

    print("[INFO] Installing required packages...")
    try:
        subprocess.check_call([
            sys.executable, "-m", "pip", "install",
            "--only-binary", ":all:",   # never compile from source
            "-r", str(requirements_file),
            "--quiet",
        ])
        print("[SUCCESS] Packages installed successfully")
        return True
    except subprocess.CalledProcessError:
        # --only-binary may reject some packages; fall back without it
        print("[INFO] Retrying without binary-only restriction...")
        try:
            subprocess.check_call([
                sys.executable, "-m", "pip", "install",
                "-r", str(requirements_file),
                "--quiet",
            ])
            print("[SUCCESS] Packages installed successfully")
            return True
        except subprocess.CalledProcessError as e:
            print(f"[ERROR] Failed to install packages: {e}")
            return False


def test_imports():
    """Verify all required modules can be imported."""
    print("[INFO] Testing application components...")
    try:
        from PyQt5.QtWidgets import QApplication
        import matplotlib
        print("[SUCCESS] All required modules found")
        return True
    except ImportError as e:
        print(f"[ERROR] Missing module: {e}")
        print("        Try: python -m pip install PyQt5 matplotlib")
        return False


def setup_desktop_app():
    """Set up the application as a desktop app."""
    print("Setting up Student Management System as desktop application...")
    print()

    if not install_requirements():
        return False

    if not test_imports():
        return False

    print()
    print("[SUCCESS] Setup complete!")
    print()
    print("To launch the application:")
    print("   python main.py")
    print("   — or double-click start.bat")
    print()
    print("Default login credentials:")
    print("   Admin   : username=admin,     password=admin123")
    print("   Teacher : username=t.ali,     password=teach001")
    print("   Student : username=s.akua1,   password=stu001")
    print("   Parent  : username=p.amoah1,  password=par1")
    return True


if __name__ == "__main__":
    try:
        success = setup_desktop_app()
        print("\n[SUCCESS] Installation successful!" if success
              else "\n[ERROR] Installation failed!")
    except KeyboardInterrupt:
        print("\n[INFO] Installation cancelled by user")
    except Exception as e:
        print(f"\n[ERROR] Unexpected error: {e}")

    input("\nPress Enter to exit...")
