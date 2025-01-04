cd %~dp0
set CURRENT_DIR=%~dp0
set CURRENT_FILE=%~dp0dmpath.exe

echo CURRENT_DIR: %CURRENT_DIR%
echo CURRENT_FILE: %CURRENT_FILE%

dmpath.exe --addpath="%CURRENT_DIR%"

pause