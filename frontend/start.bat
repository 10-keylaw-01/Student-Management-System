@echo off
REM Start script for Student Management System Frontend

cd /d "%~dp0"

echo.
echo ========================================
echo Starting Student Management System
echo ========================================
echo.

python main.py

if errorlevel 1 (
    echo.
    echo ERROR: Application failed to start
    echo.
    pause
)
