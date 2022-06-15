@echo off
echo Entering the source directory
cd ..\java\src
echo Building com.openisl.java
"%JAVA_SDK_PATH%\bin\javac.exe" -d ..\bin com\openisl\java\CISLConnect.java
"%JAVA_SDK_PATH%\bin\javac.exe" -d ..\bin com\openisl\java\CISLData.java
"%JAVA_SDK_PATH%\bin\javac.exe" -d ..\bin com\openisl\java\CISLDoubleData.java
"%JAVA_SDK_PATH%\bin\javac.exe" -d ..\bin com\openisl\java\CISLIntData.java
"%JAVA_SDK_PATH%\bin\javac.exe" -d ..\bin com\openisl\java\CISLStringData.java
echo Entering the binary directory
cd ..\bin
echo Archiving com.openisl.java
"%JAVA_SDK_PATH%\bin\jar.exe" cvf ..\..\project\com.openisl.java.jar com
echo Entering project directory
cd ..\..\project
