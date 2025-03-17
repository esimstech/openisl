@echo off
call "%~dp000_env.bat"
call "%PYTHON_VENV_PATH%\Scripts\activate.bat"
jupyter-lab
