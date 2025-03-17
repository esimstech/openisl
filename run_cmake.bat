@echo off

set ROOT_PATH=%~dp0
set ROOT_PATH=%ROOT_PATH:~0,-1%

cd "%ROOT_PATH%"

call env.bat

echo Cleaning build and install directories
for %%x in ("build\x64","build\x86","install") do (
	del /s /f /q "%%~x\*.*" 2> NUL
	for /f %%f in ('dir /ad /b "%%~x"') do rd /s /q "%%~x\%%f" 2> NUL
	rd "%%~x" 2> NUL
)

mkdir build 2> NUL
mkdir build\x64 2> NUL
mkdir build\x86 2> NUL
mkdir install 2> NUL

cd "%ROOT_PATH%\build\x64"
echo Configuring the x64 platform
"%CMAKE_PATH%\bin\cmake" -G "Visual Studio 17 2022" -A x64 -D OPENISL_JAVA_API:BOOL=TRUE -D OPENISL_PYTHON_API:BOOL=TRUE -D OPENISL_FMI:BOOL=TRUE -D OPENISL_LUA:BOOL=TRUE -D OPENISL_MODELICA:BOOL=TRUE -D OPENISL_OMNETPP:BOOL=TRUE "%ROOT_PATH%"  > "%ROOT_PATH%\run_cmake.log" 2>&1
echo Building the x64 platform
"%CMAKE_PATH%\bin\cmake" --build . --config Release  >> "%ROOT_PATH%\run_cmake.log" 2>&1
echo Installing the x64 platform
"%CMAKE_PATH%\bin\cmake" --install . --config Release  >> "%ROOT_PATH%\run_cmake.log" 2>&1

cd "%ROOT_PATH%\build\x86"
echo Configuring the x86 platform
"%CMAKE_PATH%\bin\cmake" -G "Visual Studio 17 2022" -A Win32 -D OPENISL_JAVA_API:BOOL=TRUE -D OPENISL_PYTHON_API:BOOL=TRUE -D OPENISL_FMI:BOOL=TRUE -D OPENISL_LUA:BOOL=TRUE -D OPENISL_MODELICA:BOOL=TRUE -D OPENISL_OMNETPP:BOOL=TRUE "%ROOT_PATH%"  >> "%ROOT_PATH%\run_cmake.log" 2>&1
echo Building the x86 platform
"%CMAKE_PATH%\bin\cmake" --build . --config Release  >> "%ROOT_PATH%\run_cmake.log" 2>&1
echo Installing the x86 platform
"%CMAKE_PATH%\bin\cmake" --install . --config Release  >> "%ROOT_PATH%\run_cmake.log" 2>&1
