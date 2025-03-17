@echo off
call "%~dp000_env.bat"
call "%PYTHON_VENV_PATH%\Scripts\activate.bat"
python -m pip install -r requirements.txt
pause
