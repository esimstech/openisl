@echo off
set OPENISL_PATH=%~dp0..\..
set PATH=%OPENISL_PATH%\bin\x86;%PATH%

del /F process1\process1.log 2> NUL
del /F process2\process2.log 2> NUL
del /F process3\process3.log 2> NUL

start "" /D process1\x86 process1.exe
start "" /D process2\x86 process2.exe
start "" /D process3\x86 /WAIT process3.exe

find /c "Token1: loop 10" process1\process1.log
if %errorlevel% equ 0 (
    echo Info: all processes/token1 has been successfully executed
) else (
    echo Error: all processes/token1 has failed!!!
)

find /c "Token2: loop 10" process1\process1.log
if %errorlevel% equ 0 (
    echo Info: all processes/token2 has been successfully executed
) else (
    echo Error: all processes/token2 has failed!!!
)

pause
