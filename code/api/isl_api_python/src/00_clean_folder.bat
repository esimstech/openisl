@echo off

for %%x in ("build","OpenISL_Python_API.egg-info") do (
	del /s /f /q "%%~x\*.*"
	for /f %%f in ('dir /ad /b "%%~x"') do rd /s /q "%%~x\%%f"
	rd "%%~x"
)
