@echo off
echo.
echo ========================================
echo  Student Management System - Setup
echo ========================================
echo.

REM ── Check Python ──────────────────────────────────────────────
python --version >nul 2>&1
if errorlevel 1 (
    echo [ERROR] Python not found. Please install Python 3.9+ from https://python.org
    pause
    exit /b 1
)
for /f "tokens=2" %%v in ('python --version 2^>^&1') do set PYVER=%%v
echo [OK] Python %PYVER% found.

REM ── Check C++ backend ─────────────────────────────────────────
if exist "..\sms.exe" (
    echo [OK] Found sms.exe
) else if exist "..\main.exe" (
    echo [OK] Found main.exe
) else (
    echo [WARN] C++ backend not found. Build it with:
    echo        g++ -std=c++17 -o sms main.cpp
    echo.
)

REM ── Install Python packages ───────────────────────────────────
echo.
echo [INFO] Installing required packages...
python -m pip install --upgrade pip --quiet
python -m pip install -r requirements.txt --quiet

if errorlevel 1 (
    echo.
    echo [ERROR] Package installation failed.
    echo         Try running manually:
    echo         python -m pip install PyQt5 matplotlib
    pause
    exit /b 1
)

echo [OK] All packages installed.
echo.
echo ========================================
echo  Setup complete! Run start.bat to launch.
echo ========================================
echo.
pause
