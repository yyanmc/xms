@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat" x86 > NUL 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] vcvarsall.bat failed
    exit /b 1
)
echo [INFO] Environment set up successfully.
echo [INFO] Building xplatform...
"C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\MSBuild.exe" xplatform\xplatfrom.vcxproj /p:Configuration=Release /p:Platform=x64 /p:PlatformToolset=v143 /t:Build /v:minimal
if %ERRORLEVEL% NEQ 0 (
    echo [FAIL] xplatform build failed
    exit /b 1
)
echo [PASS] xplatform built successfully!
