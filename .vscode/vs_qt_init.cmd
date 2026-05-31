@echo off
REM ============================================
REM VS + Qt 6.11.0 开发环境初始化
REM ============================================

REM 加载 Visual Studio 2022 环境 (x86/Win32)
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat" x86

REM 设置 Qt 6.11.0 环境变量
set Qt6_DIR=D:\Code\qt\6.11.0\msvc2022_64\lib\cmake\Qt6
set PATH=D:\Code\qt\6.11.0\msvc2022_64\bin;%PATH%

echo.
echo [环境就绪] Visual Studio 2022 + Qt 6.11.0 (msvc2022_64)
echo.
