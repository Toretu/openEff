@echo off
REM Quick Release Script for OpenGuitar VST3 Plugins (Windows)
REM This script helps you create a new release version

setlocal enabledelayedexpansion

echo OpenGuitar VST3 Release Helper
echo ================================
echo.

REM Check if git repo is clean
git status -s > nul 2>&1
if errorlevel 1 (
    echo Error: Not a git repository
    pause
    exit /b 1
)

for /f %%i in ('git status -s') do set dirty=1
if defined dirty (
    echo Warning: You have uncommitted changes.
    git status -s
    echo.
    set /p commit="Do you want to commit them now? (y/n): "
    if /i "!commit!"=="y" (
        set /p msg="Enter commit message: "
        git add .
        git commit -m "!msg!"
    ) else (
        echo Please commit your changes before creating a release.
        pause
        exit /b 1
    )
)

REM Get current version
for /f "tokens=*" %%i in ('git describe --tags --abbrev=0 2^>nul') do set current_version=%%i
if not defined current_version set current_version=v0.0.0

echo Current version: %current_version%
echo.

REM Ask for new version
set /p new_version="Enter new version (e.g., v1.0.0): "

REM Create tag
echo.
echo Creating tag %new_version%...
git tag -a "%new_version%" -m "Release %new_version%"

echo Pushing to GitHub...
for /f "tokens=*" %%i in ('git branch --show-current') do set current_branch=%%i
git push origin !current_branch!
git push origin "%new_version%"

echo.
echo Release %new_version% created successfully!
echo.
echo GitHub Actions is now building your plugins for Windows, Linux, and macOS.
echo Check your GitHub repository Actions tab for build progress.
echo.
pause
