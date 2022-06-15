@echo off

copy %I_BUILDDIR%\*.zip %I_DISTDIR%\connect\python\
copy %I_BUILDDIR%\*.whl %I_DISTDIR%\connect\python\

copy %I_SRCDIR%\python.zip %I_DISTDIR%\connect\python\

copy %I_PROJECTDIR%\install.txt %I_DISTDIR%\connect\python\
copy %I_PROJECTDIR%\00_env.bat %I_DISTDIR%\connect\python\
copy %I_PROJECTDIR%\01_install.bat %I_DISTDIR%\connect\python\
copy %I_PROJECTDIR%\01_jupyterlab.bat %I_DISTDIR%\connect\python\
