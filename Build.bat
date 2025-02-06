@echo off
if exist ".\Build" (
echo Removing Build files..
rd /S /Q ".\Build"
timeout 1 >nul
echo Build successfully removed.
)
set current_directory=%cd%
echo Creating directory: %current_directory%\Build in current working directory.
timeout 1 >nul
mkdir %current_directory%\Build
echo Changing working directory to %current_directory%\Build ...
cd %current_directory%\Build
echo Running CMake...
cmake ..
echo Done!
pause