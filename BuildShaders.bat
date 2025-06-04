@echo off
setlocal enabledelayedexpansion

:: Find first subfolder in %USERPROFILE%\vulkan
for /f "delims=" %%a in ('dir /b /ad "%USERPROFILE%\vulkan"') do (
    set "versionName=%%a"
    goto found_version
)
echo ⚠️ No vulkan version folder found in "%USERPROFILE%\vulkan"
goto :eof

:found_version
echo 🎯 Using Vulkan version: %versionName%

:: Function to compile a shader (vertex and fragment)
:: %1 = project root, %2 = shader base name without extension
:compile_shader
set "project_root=%~1"
set "shader_base=%~2"

set "glslc=%USERPROFILE%\vulkan\%versionName%\x86_64\bin\glslc.exe"

echo 🎯 Compiling shader %shader_base%...

"%glslc%" -O "%project_root%\Assets\Shaders\%shader_base%.vert" -o "%project_root%\Assets\Shaders\output\%shader_base%.vert.spv"
"%glslc%" -O "%project_root%\Assets\Shaders\%shader_base%.frag" -o "%project_root%\Assets\Shaders\output\%shader_base%.frag.spv"
goto :eof

:: Function to compile all shaders
:: %1 = project root
:compile_shaders
set "project_root=%~1"
if not exist "%project_root%\Assets\Shaders\output" mkdir "%project_root%\Assets\Shaders\output"

echo 🛠️ Starting shader compilation...

for %%f in ("%project_root%\Assets\Shaders\*.vert" "%project_root%\Assets\Shaders\*.frag") do (
    if exist "%%f" (
        set "filename=%%~nxf"
        echo 🔹 Compiling %%~nxf
        call :compile_shader "%project_root%" "%%~nxf"
    )
)

echo ✅ Done compiling shaders!
goto :eof

:: Main script starts here

if "%~1"=="" (
    echo ⚠️ Usage: %~nx0 ^<project_root_path^>
    goto :eof
)

echo 🚀 Compiling shaders in %~1

echo 📂 Files found:
dir "%~1\Assets\Shaders\*.vert" "%~1\Assets\Shaders\*.frag" /b

call :compile_shaders "%~1"

echo 📋 Copying assets
xcopy /e /i /y "%~1\Assets" "%~1\build\bin\Assets"
echo 🎉 Done copying assets
