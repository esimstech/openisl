#! /bin/bash

ROOT_PATH=`pwd`

echo Cleaning build and install directories
if [ -d build ]; then
  cd build
  rm -rf x64 2> /dev/null
  cd ..
fi
rm -rf install

mkdir build 2> /dev/null
cd build
mkdir x64 2> /dev/null
cd ..
mkdir install 2> /dev/null

cd build/x64
echo Configuring the x64 platform
cmake -G "Unix Makefiles" -D OPENISL_JAVA_API:BOOL=TRUE -D OPENISL_PYTHON_API:BOOL=TRUE "$ROOT_PATH"  > "$ROOT_PATH/run_cmake.log" 2>&1
echo Building the x64 platform
cmake --build . --config Release  >> "$ROOT_PATH/run_cmake.log" 2>&1
echo Installing the x64 platform
cmake --install . --config Release  >> "$ROOT_PATH/run_cmake.log" 2>&1
