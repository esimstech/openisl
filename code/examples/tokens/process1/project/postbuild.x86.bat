@echo off

copy %I_TARGETNAME%.exe %I_DISTDIR%\examples\tokens\process1\
copy %I_PROJECTDIR%process1.xml %I_DISTDIR%\examples\tokens\process1\
copy %I_PROJECTDIR%..\..\main\*.* %I_DISTDIR%\examples\tokens\
