@echo off

set BUILD_PATH=%~dp0
call "%BUILD_PATH%islfmuenv.bat"

set CURRENT_PATH=%CD%
cd %BUILD_PATH%

if not exist build goto BUILD_CLEAN
echo Removing build directory..
rd /S /Q build
:BUILD_CLEAN
mkdir build

:: check if ISL x86 DLL exists
if exist "%OPENISL_PATH%\connect\fmi\lib\x86\isl_slavefmi.dll" goto DLL_X86_EXISTS
echo ERROR: ISL file 'isl_slavefmi.dll' expected in directory '%OPENISL_PATH%\connect\fmi\lib\x86', but does not exist.
exit /b 1
:DLL_X86_EXISTS

:: check if ISL x64 DLL exists
if exist "%OPENISL_PATH%\connect\fmi\lib\x64\isl_slavefmi.dll" goto DLL_X64_EXISTS
echo ERROR: ISL file 'isl_slavefmi.dll' expected in directory '%OPENISL_PATH%\connect\fmi\lib\x64', but does not exist.
exit /b 1
:DLL_X64_EXISTS

:: remove target FMU if it exists
if not exist "%ISLFMU_PATH%\%ISLFMU_NAME%.fmu" goto FMU_REMOVED
echo Removing existing FMU file '%ISLFMU_PATH%\%ISLFMU_NAME%.fmu'
set TMPPATH=%CD%
cd %ISLFMU_PATH%
del /F /S /Q "%ISLFMU_NAME%.fmu"
if exist "%ISLFMU_NAME%" rd /S /Q "%ISLFMU_NAME%"
cd %TMPPATH%
:FMU_REMOVED

:: copy ISL DLL
mkdir build\binaries\win32
mkdir build\binaries\win64
copy "%OPENISL_PATH%\connect\fmi\lib\x86\isl_slavefmi.dll" "build\binaries\win32\%ISLFMU_NAME%.dll"
copy "%OPENISL_PATH%\bin\x86\isl_api.dll" "build\binaries\win32\isl_api.dll"
copy "%OPENISL_PATH%\connect\fmi\lib\x64\isl_slavefmi.dll" "build\binaries\win64\%ISLFMU_NAME%.dll"
copy "%OPENISL_PATH%\bin\x64\isl_api.dll" "build\binaries\win64\isl_api.dll"
:: copy modelDescription file
copy src\modelDescription.xml build\
echo FMU directory structure and files have been created

::create zip archive
echo Creating archive '%ISLFMU_PATH%\%ISLFMU_NAME%.zip'
cd build
"%SEVENZIP_PATH%\7z.exe" a "%ISLFMU_PATH%\%ISLFMU_NAME%.zip" .
cd ..

echo Renaming archive to '%ISLFMU_PATH%\%ISLFMU_NAME%.fmu'
rename "%ISLFMU_PATH%\%ISLFMU_NAME%.zip" "%ISLFMU_NAME%.fmu"

cd %CURRENT_PATH%

:: exit build script
exit /b 0
