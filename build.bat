@echo off
echo Building Student Management System...

REM Check for g++ compiler
where g++ >nul 2>&1
if %errorlevel% == 0 (
    echo Using g++ compiler...
    g++ -std=c++17 -I. -Wall -Wextra -static-libgcc -static-libstdc++ -static -lpthread main.cpp -o sms.exe
    if %errorlevel% == 0 (
        echo Build successful! Run StudentManagementSystem.exe to start.
    ) else (
        echo Build failed with g++
    )
    goto :end
)

REM Check for cl compiler (Visual Studio)
where cl >nul 2>&1
if %errorlevel% == 0 (
    echo Using Microsoft Visual C++ compiler...
    cl /EHsc /std:c++17 /I. main.cpp /Fe:StudentManagementSystem.exe
    if %errorlevel% == 0 (
        echo Build successful! Run StudentManagementSystem.exe to start.
    ) else (
        echo Build failed with cl
    )
    goto :end
)

REM Check for clang++
where clang++ >nul 2>&1
if %errorlevel% == 0 (
    echo Using clang++ compiler...
    clang++ -std=c++17 -I. -Wall -Wextra main.cpp -o StudentManagementSystem.exe
    if %errorlevel% == 0 (
        echo Build successful! Run StudentManagementSystem.exe to start.
    ) else (
        echo Build failed with clang++
    )
    goto :end
)

echo No C++ compiler found. Please install:
echo - MinGW-w64 (for g++)
echo - Visual Studio Build Tools (for cl)
echo - LLVM/Clang (for clang++)

:end
pause