@echo off
setlocal enabledelayedexpansion

:: ===========================================================================
::  Lacus one-click build script (CMake + Visual Studio generator)
::
::  Usage:  build.bat [Debug|Release|RelWithDebInfo|MinSizeRel] [-nopause]
::
::    build.bat                       build Debug (default)
::    build.bat Release               build Release
::    build.bat Release -nopause      do not wait for a key when finished
::
::  Intermediate files : <root>\build
::  Executable         : <root>\bin\<Config>\Lacus.exe
::
::  Delete the build directory if you want a clean configuration.
:: ===========================================================================

set "ROOT=%~dp0"
if "%ROOT:~-1%"=="\" set "ROOT=%ROOT:~0,-1%"
set "BUILD_DIR=%ROOT%\build"
set "BIN_DIR=%ROOT%\bin"
set "PF86=%ProgramFiles(x86)%"
set "PF=%ProgramFiles%"
set "EXIT_CODE=0"

set "CONFIG=%~1"
if /i "%CONFIG%"=="-nopause" (
    set "CONFIG="
    set "NOPAUSE=-nopause"
) else (
    set "NOPAUSE=%~2"
)

if "%CONFIG%"=="" set "CONFIG=Debug"

if /i not "%CONFIG%"=="Debug" if /i not "%CONFIG%"=="Release" if /i not "%CONFIG%"=="RelWithDebInfo" if /i not "%CONFIG%"=="MinSizeRel" (
    echo [ERROR] Unknown configuration: "%CONFIG%"
    echo         Available: Debug / Release / RelWithDebInfo / MinSizeRel
    set "EXIT_CODE=1"
    goto :finish
)

:: ---- locate cmake ---------------------------------------------------------
set "CMAKE_EXE="
for /f "delims=" %%i in ('where cmake 2^>nul') do (
    if not defined CMAKE_EXE set "CMAKE_EXE=%%i"
)

if not defined CMAKE_EXE (
    set "VSINSTALL="
    if exist "!PF86!\Microsoft Visual Studio\Installer\vswhere.exe" (
        for /f "usebackq tokens=*" %%i in (`"!PF86!\Microsoft Visual Studio\Installer\vswhere.exe" -version 17 -property installationPath`) do set "VSINSTALL=%%i"
    )
    if not defined VSINSTALL (
        for %%p in (Community Professional Enterprise BuildTools) do (
            if exist "!PF!\Microsoft Visual Studio\2022\%%p" set "VSINSTALL=!PF!\Microsoft Visual Studio\2022\%%p"
        )
    )
    if defined VSINSTALL (
        if exist "!VSINSTALL!\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" (
            set "CMAKE_EXE=!VSINSTALL!\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
        )
    )
)

if not defined CMAKE_EXE (
    echo [ERROR] cmake was not found in PATH or in Visual Studio 2022.
    echo         Install CMake or run this script from a VS developer prompt.
    set "EXIT_CODE=1"
    goto :finish
)

echo [INFO] cmake     : !CMAKE_EXE!
echo [INFO] source    : %ROOT%
echo [INFO] build dir : %BUILD_DIR%
echo [INFO] config    : %CONFIG%
echo.

:: ---- configure -----------------------------------------------------------
if not exist "%BUILD_DIR%\CMakeCache.txt" (
    echo [INFO] No build cache found, configuring from scratch ...
    "!CMAKE_EXE!" -S "%ROOT%" -B "%BUILD_DIR%" -G "Visual Studio 17 2022" -A x64
    if errorlevel 1 (
        echo [WARN] Generator "Visual Studio 17 2022" is not usable, retrying with the CMake default generator ...
        if exist "%BUILD_DIR%\CMakeCache.txt" del /q "%BUILD_DIR%\CMakeCache.txt" >nul 2>nul
        if exist "%BUILD_DIR%\CMakeFiles" rd /s /q "%BUILD_DIR%\CMakeFiles" >nul 2>nul
        "!CMAKE_EXE!" -S "%ROOT%" -B "%BUILD_DIR%"
        if errorlevel 1 (
            echo [ERROR] CMake configure failed.
            set "EXIT_CODE=1"
            goto :finish
        )
    )
) else (
    echo [INFO] Updating build files ...
    "!CMAKE_EXE!" -S "%ROOT%" -B "%BUILD_DIR%"
    if errorlevel 1 (
        echo [ERROR] CMake configure failed.
        set "EXIT_CODE=1"
        goto :finish
    )
)

:: ---- build ---------------------------------------------------------------
echo.
echo [INFO] Building, this may take a while ...
"!CMAKE_EXE!" --build "%BUILD_DIR%" --config %CONFIG% --parallel
if errorlevel 1 (
    set "EXIT_CODE=!errorlevel!"
    goto :finish
)

echo.
echo [OK] Build succeeded.
if exist "%BIN_DIR%\%CONFIG%\Lacus.exe" (
    echo      Executable : %BIN_DIR%\%CONFIG%\Lacus.exe
) else (
    echo      Executable : not found under %BIN_DIR%\%CONFIG%, please check the target output name.
)
echo      Build files: %BUILD_DIR%

:finish
echo.
if not "%EXIT_CODE%"=="0" echo [FAILED] Build failed, exit code %EXIT_CODE%.

:: wait for a key only when the script was started by double-click
if /i not "%NOPAUSE%"=="-nopause" (
    echo(!cmdcmdline! | find /i "%~nx0" >nul
    if not errorlevel 1 (
        echo.
        pause
    )
)

exit /b %EXIT_CODE%
