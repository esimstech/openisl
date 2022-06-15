@echo off

copy %I_TARGETNAME%.dll %I_DISTDIR%\bin\win32\

copy %I_LIBRARYDIR%\com.openisl.java.jar %I_DISTDIR%\connect\java\
