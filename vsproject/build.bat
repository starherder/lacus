@echo off
setlocal enabledelayedexpansion

set "VSPROJECT_DIR=%~dp0"
set "BUILD_DIR=%~dp0..\build"
set "SOLUTION=%VSPROJECT_DIR%lacus.sln"

:: Parse arguments: build.bat [Debug|Release] [Win32|x64]
if "%~1"=="" (
    set "CONFIG=Debug"
    set "PLATFORM=x64"
) else (
    set "CONFIG=%~1"
    if "%~2"=="" (set "PLATFORM=x64") else (set "PLATFORM=%~2")
)

:: Detect VS2022 installation via vswhere or fallback paths
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
set "VSINSTALL="
if exist "%VSWHERE%" (
    for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -version 17 -property installationPath`) do set "VSINSTALL=%%i"
)
if not defined VSINSTALL (
    for %%p in (Community Professional Enterprise) do (
        if exist "%ProgramFiles%\Microsoft Visual Studio\2022\%%p" (
            set "VSINSTALL=%ProgramFiles%\Microsoft Visual Studio\2022\%%p"
        )
    )
)
if not defined VSINSTALL (
    echo Error: Visual Studio 2022 not found.
    exit /b 1
)

:: Initialize VS developer environment
call "%VSINSTALL%\VC\Auxiliary\Build\vcvarsall.bat" %PLATFORM% >nul 2>&1
if %errorlevel% neq 0 (
    echo Error: failed to set up environment for %PLATFORM%.
    exit /b 1
)

:: Clean previous build
if exist "%BUILD_DIR%" rmdir /s /q "%BUILD_DIR%"
mkdir "%BUILD_DIR%"

:: Build
echo Building %CONFIG% %PLATFORM% ...
msbuild "%SOLUTION%" ^
    /p:Configuration=%CONFIG% ^
    /p:Platform=%PLATFORM% ^
    /p:OutDir="%BUILD_DIR%\\" ^
    /p:IntDir="%BUILD_DIR%\\obj\\%CONFIG%_%PLATFORM%\\" ^
    /m ^
    /nologo

if %errorlevel% equ 0 (
    echo.
    echo Build succeeded. Copying lacus.exe to project root ...
    copy /y "%BUILD_DIR%\lacus.exe" "%~dp0.." >nul
    echo Output: %BUILD_DIR%
    echo Binary: %~dp0..\lacus.exe
) else (
    echo.
    echo Build failed.
)
