"""Setup script for Student Management System desktop application."""

import subprocess
import sys
from pathlib import Path


def install_requirements():
    requirements_file = Path(__file__).parent / "requirements.txt"
    if not requirements_file.exists():
        print("[ERROR] requirements.txt not found")
        return False
    print("[INFO] Installing required packages...")
    try:
        subprocess.check_call([
            sys.executable, "-m", "pip", "install",
            "--only-binary", ":all:",
            "-r", str(requirements_file), "--quiet",
        ])
        print("[SUCCESS] Packages installed successfully")
        return True
    except subprocess.CalledProcessError:
        print("[INFO] Retrying without binary-only restriction...")
        try:
            subprocess.check_call([
                sys.executable, "-m", "pip", "install",
                "-r", str(requirements_file), "--quiet",
            ])
            print("[SUCCESS] Packages installed successfully")
            return True
        except subprocess.CalledProcessError as e:
            print(f"[ERROR] Failed to install packages: {e}")
            return False


def test_imports():
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


def create_desktop_shortcut():
    """Create a proper .lnk shortcut with icon via PowerShell, fallback to .bat."""
    # Use the shell's Desktop path — works correctly whether OneDrive is active or not
    import subprocess
    result = subprocess.run(
        ['powershell', '-Command',
         '[Environment]::GetFolderPath("Desktop")'],
        capture_output=True, text=True
    )
    desktop     = Path(result.stdout.strip()) if result.returncode == 0 else Path.home() / 'Desktop'
    current_dir = Path(__file__).parent.absolute()
    ico_path    = current_dir / 'resources' / 'icons' / 'app_icon.ico'
    launcher    = current_dir / 'launch_sms.bat'
    lnk         = desktop / 'Student Management System.lnk'

    # Generate icon if missing
    if not ico_path.exists():
        try:
            subprocess.run([sys.executable, str(current_dir / "generate_icon.py")],
                           check=True, capture_output=True)
        except Exception:
            pass

    # Try PowerShell .lnk (shows proper icon on Desktop)
    icon_line = f'$s.IconLocation="{ico_path}";' if ico_path.exists() else ''
    ps = (
        f'$s=(New-Object -COM WScript.Shell).CreateShortcut("{lnk}");'
        f'$s.TargetPath="{launcher}";'
        f'$s.WorkingDirectory="{current_dir}";'
        f'$s.Description="Student Management System";'
        f'{icon_line}'
        f'$s.Save()'
    )
    try:
        subprocess.run(["powershell", "-Command", ps],
                       capture_output=True, timeout=10, check=True)
        print(f"[SUCCESS] Desktop shortcut created: {lnk}")
        return
    except Exception:
        pass

    # Fallback: plain .bat file
    bat = desktop / "Student Management System.bat"
    bat.write_text(
        f'@echo off\ntitle Student Management System\ncd /d "{current_dir}"\npython main.py\n',
        encoding='utf-8'
    )
    print(f"[SUCCESS] Desktop shortcut created: {bat}")


def setup_desktop_app():
    print("Setting up Student Management System as desktop application...")
    print()

    if not install_requirements():
        return False

    if not test_imports():
        return False

    try:
        create_desktop_shortcut()
    except Exception as e:
        print(f"[WARN] Could not create desktop shortcut: {e}")

    print()
    print("[SUCCESS] Setup complete!")
    print()
    print("To launch: double-click 'Student Management System' on your Desktop")
    print("        or run: python main.py")
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
