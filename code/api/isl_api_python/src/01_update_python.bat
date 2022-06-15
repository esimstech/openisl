@echo off
call %~dp000_env.bat
python -m pip install --upgrade pip
python -m pip install -r requirements.txt
pause
