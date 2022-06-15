# OpenISL: An open source co-simulation API (Application Programming Interface)
Welcome to the OpenISL a Open Source Project offering a co-simulation API. If you are interested in actively particpating in the project, please contact us at [support@esims.tech](mailto:support@esims.tech)

## What is OpenISL?
OpenISL is an API natively developped in C++ used to enable dynamic data exchange on coupled numerical simulations using multiple methods of synchronization depending on the simulation software constraints and depending the contextual aspect of use.

OpenISL is also available with Java, .NET and Python languages.

For more information on co-simulation: https://en.wikipedia.org/wiki/Co-simulation

## Why should I use OpenISL?
Use OpenISL for:
* co-simulation ;)

## How to start with OpenISL?
* Prerequisites:
    * Install Visual Studio 2017 community version: https://visualstudio.microsoft.com/fr/vs/older-downloads/
    * Install Boost 1.77: https://www.boost.org/users/history/version_1_77_0.html
    * (Only required for the OpenISL Java API) Install Java Development Kit: https://www.oracle.com/java/technologies/downloads/
    * (Only required for the OpenISL Python API) Install Python 3.8.10 (https://www.python.org/downloads/release/python-3810/) in the directory: `code\api\isl_api_python\src\python`

* Edit the file `code\envvars.props` to set the paths to access the Boost library and Java
    * Example:
```xml
    <BOOST_LIB_PATH>F:\Dev\boost_1_77_0</BOOST_LIB_PATH>
    <JAVA_SDK_PATH>C:\Program Files\Java\jdk1.8.0_201</JAVA_SDK_PATH>
```

* Open the project `build\clean\isl_clean.sln`, and build it for one of the release configuration (x86 or x64): a folders structure under `dist\source` will be created.

* Open the project `build\openisl.sln`, and build it for the x86 release configuration, and then for the x64 release configuration. ISL librraries and examples will be built.

* OpenISL has then been installed in the folder `dist\source`.

* Examples can be executed from the folder `dist\source\examples`

## Latest release
* `OpenISL 2022.06 v1.0.0 x64 Setup.exe` is available for download
* This is the first release of OpenISL compatible with 4 languages C/C++, .NET, Java and Python
* Simulation environments like OpenModelica, SciLab are not yet integrated. See the section below for incoming features.

## Next steps
Here is a list of simulation environments and other features that are planned be implemented with OpenISL in the next months:
* Phase 1 (2022 TBD):
    * Linux version
    * FMI 2.0 / 3.0 (Slave and Master)
    * OpenModelica
    * OMNet++
* Phase 2 (2023 TBD)
    * OpenMatrix
    * Scilab
    * LUA Scripting
