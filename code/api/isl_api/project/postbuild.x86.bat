@echo off

copy %I_TARGETNAME%.dll %I_DISTDIR%\bin\win32\
copy %I_TARGETNAME%.lib %I_DISTDIR%\bin\win32\

copy %I_INCLUDEDIR%\isl_api_global.h %I_DISTDIR%\inc\
copy %I_INCLUDEDIR%\isl_api.h %I_DISTDIR%\inc\
copy %I_INCLUDEDIR%\isl_const.h %I_DISTDIR%\inc\
copy %I_INCLUDEDIR%\isl_connect.h %I_DISTDIR%\inc\
copy %I_INCLUDEDIR%\isl_errorcodes.h %I_DISTDIR%\inc\
copy %I_INCLUDEDIR%\isl_instances.h %I_DISTDIR%\inc\
copy %I_INCLUDEDIR%\isl_variable.h %I_DISTDIR%\inc\
copy %I_INCLUDEDIR%\isl_data.h %I_DISTDIR%\inc\
copy %I_INCLUDEDIR%\isl_simulations.h %I_DISTDIR%\inc\
copy %I_INCLUDEDIR%\isl_utils.h %I_DISTDIR%\inc\
copy %I_INCLUDEDIR%\swversion.h %I_DISTDIR%\inc\
copy %I_INCLUDEDIR%\isl_api_c.h %I_DISTDIR%\inc\

copy %I_RESOURCEDIR%\isl.ico %I_DISTDIR%\resources\icons\