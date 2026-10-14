@echo off
setlocal enabledelayedexpansion

:: ===========================================================================
::  Lacus one-click run script
::
::  Usage:  run.bat [Debug|Release|RelWithDebInfo|MinSizeRel] [-nopause]
::
::    run.bat                       run Debug (default)
::    run.bat Release               run Release
::    run.bat Debug -nopause        do not wait for a key when finished
::
::  Executable : <root>\bin\<Config>\Lacus.exe
::  Working dir: <root>
::
::  The working directory is kept at the project root so res/system.json,
::  res/*, and log/* are resolved consistently.
:: ===========================================================================

set "ROOT=%~dp0"
if "%ROOT:~-1%"=="\" set "ROOT=%ROOT:~0,-1%"
set "BIN_DIR=%ROOT%\bin"
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

set "EXE=%BIN_DIR%\%CONFIG%\Lacus.exe"

echo [INFO] root      : %ROOT%
echo [INFO] config    : %CONFIG%
echo [INFO] executable: %EXE%
echo.

if not exist "%EXE%" (
    echo [ERROR] Executable not found.
    echo         Please build first: build.bat %CONFIG%
    set "EXIT_CODE=1"
    goto :finish
)

pushd "%ROOT%" >nul
echo [INFO] Running Lacus ...
"%EXE%"
set "EXIT_CODE=!errorlevel!"
popd >nul

:finish
echo.
if "%EXIT_CODE%"=="0" (
    echo [OK] Run finished.
) else (
    echo [FAILED] Run failed, exit code %EXIT_CODE%.
)

:: wait for a key only when the script was started by double-click
if /i not "%NOPAUSE%"=="-nopause" (
    echo(!cmdcmdline! | find /i "%~nx0" >nul
    if not errorlevel 1 (
        echo.
        pause
    )
)

exit /b %EXIT_CODE%
