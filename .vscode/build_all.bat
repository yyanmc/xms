@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat" x64 > NUL 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] vcvarsall.bat failed
    exit /b 1
)
echo [INFO] Building xplatform...
MSBuild.exe xplatform\xplatfrom.vcxproj /p:Configuration=Release /p:Platform=x64 /p:PlatformToolset=v143 /t:Build /v:minimal
if %ERRORLEVEL% NEQ 0 (
    echo [FAIL] xplatform build failed
    exit /b 1
)
echo [PASS] xplatform built successfully!

echo [INFO] Building upload service...
MSBuild.exe xms_upload_service\xms_upload_service.vcxproj /p:Configuration=Release /p:Platform=x64 /p:PlatformToolset=v143 /t:Build /v:minimal
if %ERRORLEVEL% NEQ 0 (
    echo [FAIL] upload service build failed
    exit /b 1
)
echo [PASS] upload service built successfully!

echo [INFO] Building download service...
MSBuild.exe xms_download_service\xms_download_service.vcxproj /p:Configuration=Release /p:Platform=x64 /p:PlatformToolset=v143 /t:Build /v:minimal
if %ERRORLEVEL% NEQ 0 (
    echo [FAIL] download service build failed
    exit /b 1
)
echo [PASS] download service built successfully!

echo [INFO] Building client GUI...
MSBuild.exe xms_disk_client_gui\xms_disk_client_gui.sln /p:Configuration=Release /p:Platform=x64 /p:PlatformToolset=v143 /t:Build /v:minimal
if %ERRORLEVEL% NEQ 0 (
    echo [FAIL] client GUI build failed
    exit /b 1
)
echo [PASS] client GUI built successfully!

echo [ALL PASS] All projects built successfully!
