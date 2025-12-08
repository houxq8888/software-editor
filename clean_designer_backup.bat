@echo off
chcp 65001 >nul

echo Cleaning Qt Designer backup files...
echo.

echo Step 1: Check for running Qt Designer processes
tasklist /fi "imagename eq designer.exe" | find "designer.exe"
if %errorlevel% equ 0 (
    echo Killing Qt Designer processes...
    taskkill /f /im designer.exe
)

echo.
echo Step 2: Clean registry backup settings
reg delete "HKEY_CURRENT_USER\Software\QtProject\Qt Designer" /v "backup/fileListOrg" /f 2>nul
reg delete "HKEY_CURRENT_USER\Software\QtProject\Qt Designer" /v "backup/fileListBak" /f 2>nul
echo Registry cleaned.

echo.
echo Step 3: Clean temporary files
forfiles /p "%TEMP%" /s /m *designer* /c "cmd /c del /q @path" 2>nul
forfiles /p "%TEMP%" /s /m *backup* /c "cmd /c del /q @path" 2>nul
echo Temporary files cleaned.

echo.
echo Step 4: Clean AppData files
if exist "%APPDATA%\QtProject\Qt Designer" (
    del /q "%APPDATA%\QtProject\Qt Designer\*backup*" 2>nul
    del /q "%APPDATA%\QtProject\Qt Designer\*designer*" 2>nul
    echo AppData files cleaned.
)

echo.
echo Qt Designer backup files have been cleaned successfully!
echo You can now start Qt Designer without the session error message.
pause