@echo off
echo Entering the source directory
cd ..\java\src
echo Building the C header 
"%JAVA_SDK_PATH%\bin\javah.exe" -o ..\..\include\isl_java.h -jni -classpath ..\bin com.openisl.java.CISLConnect com.openisl.java.CISLData com.openisl.java.CISLDoubleData com.openisl.java.CISLIntData com.openisl.java.CISLStringData
echo Reinitialize the current directory
cd ..\..\project
