@echo off
call 00_env.bat
echo "Cleaning the distribution directory"
mkdir dist 2> NUL 
del /F /Q dist\*
echo "Building a source distribution"
python setup.py sdist --formats=zip
if exist "dist\OpenISL-Python-API*.zip" echo "Ok" > dist\build_src.txt
echo "Building a wheel distribution"
python setup.py bdist_wheel
if exist "dist\OpenISL_Python_API*.whl" echo "Ok" > dist\build_wheel.txt

