@echo off

REM 32-bits binary files
mkdir bin 2> NUL
mkdir bin\win32 2> NUL
echo ######## Removing 32-bits binary files ########
del /F /Q bin\win32\*

REM 64-bits binary files
mkdir bin 2> NUL
mkdir bin\win64 2> NUL
echo ######## Removing 64-bits binary files ########
del /F /Q bin\win64\*

REM include files
mkdir inc 2> NUL
echo ######## Removing include files ########
del /F /Q inc\*

REM resource files
mkdir resources 2> NUL
mkdir resources\icons 2> NUL
echo ######## Removing resource files ########
del /F /Q resources\icons\*

REM ini file
echo ######## Removing ini file ########
del /F /Q isl_api.ini

REM connect files
mkdir connect 2> NUL
mkdir connect\java 2> NUL
mkdir connect\python 2> NUL
echo ######## Removing connect files ########
del /F /Q connect\java\*
del /F /Q connect\python\*

REM examples files
mkdir examples 2> NUL
mkdir examples\tokens 2> NUL
mkdir examples\tokens\process1 2> NUL
mkdir examples\tokens\process2 2> NUL
mkdir examples\tokens\process3 2> NUL

echo ######## Removing examples files ########
del /F /Q examples\tokens\*
del /F /Q examples\tokens\process1\*
del /F /Q examples\tokens\process2\*
del /F /Q examples\tokens\process3\*

echo "cleaned" > output.txt