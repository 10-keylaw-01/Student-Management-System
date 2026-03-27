@echo off
REM Setup script for Student Management System Frontend

echo.
echo ========================================
echo Student Management System - Setup
echo ========================================
echo.

echo Checking Python installation...
python --version >nul 2>&1
if errorlevel 1 (
    echo ERROR: Python not found. Please install Python 3.8+
    pause
    exit /b 1
)

echo Checking if C++ backend is built...
if not exist "..\sms.exe" (
    if not exist "..\main.exe" (
        echo WARNING: C++ backend executable not found!
        echo Please build the C++ backend first:
        echo   g++ -std=c++17 -o sms main.cpp
        echo.
    ) else (
        echo Found main.exe
    )
) else (
    echo Found sms.exe
)

echo.
echo Installing Python dependencies...
pip install -q -r requirements.txt

if errorlevel 1 (
    echo ERROR: Failed to install dependencies
    pause
    exit /b 1
)

echo.
echo ========================================
echo Setup complete! Run 'start.bat' to launch the application.
echo ========================================
echo.
pause
