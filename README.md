# OpenISL: An open source co-simulation API (Application Programming Interface)
Welcome to the OpenISL a Open Source Project offering a co-simulation API.

## What is OpenISL?
OpenISL is an API natively developped in C++ used to enable dynamic data exchange on coupled numerical simulations using multiple methods of synchronization depending on the simulation software constraints and depending the contextual aspect of use.

OpenISL is also available with Java, .NET and Python languages.

For more information on co-simulation: https://en.wikipedia.org/wiki/Co-simulation

## Why should I use OpenISL?
Use OpenISL for:
* co-simulation ;)

## How to start with OpenISL?
* Prerequisites:
    * For all platforms:
		* For the API:
			* Install CMake(version 3.12 or higher version): https://cmake.org/download/
			* Install Boost 1.84: https://www.boost.org/users/history/version_1_84_0.html
			* (Only required for the OpenISL Java API, option OPENISL_JAVA_API in CMake) Install Java Development Kit: https://www.oracle.com/java/technologies/downloads/
			* (Only required for the OpenISL Python API, option OPENISL_PYTHON_API in CMake) Install Python 3.13.0 (https://www.python.org/downloads/release/python-3130/)
		* For the connectors (configured and tested on Windows only):
			* FMI v2.0 connector (master connecting a slave FMU): Install the FMI v2.0 headers files (https://fmi-standard.org/) in the folder of your choice ending with `v2.0`. Example: F:\Dev\20_Libraries\FMI_CS\v2.0.
			* LUA Language v5.3.6 (or higher version): Install the dynamic binaries with their headers (https://luabinaries.sourceforge.net/) in the folder of your choice ending with `bin.win64` or `bin.win32` depending on the downloaded version. It is recommended to install both versions. Examples : the DLL lua53.dll and the library in F:\Dev\20_Libraries\lua_5.3.6\bin.w32, and the headers in F:\Dev\20_Libraries\lua_5.3.6\bin.w32\include.
			* Modelica connector: no prerequisites are required.
			* OMNet++ connector : no prerequisites are required.
			* Matlab connector : Matlab 32 bits or 64 bits shall be installed.
			* Simulink connector : Matlab/Simulink 32 bits or 64 bits shall be installed.
			
    * On Windows:
        * Built with Visual Studio 2022 community version: https://visualstudio.microsoft.com/fr/vs/older-downloads/
    * On Linux:
        * Built on Ubuntu Server 20.04 64 bits with GCC v9.4.0

* Configure, build and install the project with CMake
    * Prerequisites
        * External packages like Boost, Java or Python could be easily detected by CMake if environment variables like `BOOSTROOT`, `JAVA_HOME` are set.
        * We recommend to create the "build" and "install" directories from the root of the project repository as follow:
            * x64 build folder: `build/x64`
            * x64 build folder: `build/x86`
            * installation folder for both architectures: `install`
        * C++ compilation environments tested with our current configuration:
            * Visual Studio 2017 community version (x64 and x86 architectures)
            * Ubuntu server 20.04 64-bits, GCC v9.4.0
		* Environment variables must be set to build the connectors:
			* FMI v2.0 connector : FMI_PATH = <Path to the FMI folder without `v2.0`>. Example :
				```shell
				set FMI_PATH=F:\Dev\20_Libraries\FMI_CS
				````
			* LUA Language v5.3.6 : LUA_PATH = <Path the root folder of the LUA installation>. Example :
				```shell
				set LUA_PATH=F:\Dev\20_Libraries\lua_5.3.6
				````
    * Example of shell script used on Windows to configure the project with CMake. The script has been started from the root directory of the repository.

```shell
@echo off

set ROOT_PATH=%~dp0
set ROOT_PATH=%ROOT_PATH:~0,-1%

set CMAKE_PATH=C:\Software\CMake
set BOOSTROOT=F:\Dev\20_Libraries\boost_1_84_0
set JAVA_HOME=C:\Program Files\Java\jdk-21

cd "%ROOT_PATH%\build\x64"
"%CMAKE_PATH%\bin\cmake" -G "Visual Studio 15 2017" -A x64 -D OPENISL_JAVA_API:BOOL=TRUE -D OPENISL_PYTHON_API:BOOL=TRUE "%ROOT_PATH%"  > "%ROOT_PATH%\run_cmake.log" 2>&1
"%CMAKE_PATH%\bin\cmake" --build . --config Release  >> "%ROOT_PATH%\run_cmake.log" 2>&1
"%CMAKE_PATH%\bin\cmake" --install . --config Release  >> "%ROOT_PATH%\run_cmake.log" 2>&1

cd "%ROOT_PATH%\build\x86"
"%CMAKE_PATH%\bin\cmake" -G "Visual Studio 15 2017" -A Win32 -D OPENISL_JAVA_API:BOOL=TRUE -D OPENISL_PYTHON_API:BOOL=TRUE "%ROOT_PATH%"  >> "%ROOT_PATH%\run_cmake.log" 2>&1
"%CMAKE_PATH%\bin\cmake" --build . --config Release  >> "%ROOT_PATH%\run_cmake.log" 2>&1
"%CMAKE_PATH%\bin\cmake" --install . --config Release  >> "%ROOT_PATH%\run_cmake.log" 2>&1
```

* OpenISL has then been installed in the folder `install`.

* Examples can be executed from the folder `install\examples`

## Latest release
* Windows and Linux binaries will be available soon.

## Roadmap summary
* The roadmap summary will be available soon.
