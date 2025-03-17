@echo off
call "%~dp000_env.bat"
call "%PYTHON_VENV_PATH%\Scripts\activate.bat"
python -m pip install dist/openisl_python_api-1.0.0.zip
