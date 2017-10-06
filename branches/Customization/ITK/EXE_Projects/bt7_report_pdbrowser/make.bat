@echo off

REM Set Teamcenter-Env
SET TC_ROOT=C:\Siemens\Teamcenter8
SET TC_DATA=C:\Siemens\tcdata
call %TC_DATA%\tc_profilevars

set MSDEV_HOME=C:\Program Files (x86)\Microsoft Visual Studio 8\VC
call "%MSDEV_HOME%\vcvarsall.bat" x86_amd64

if not exist mt.exe (
    echo Please make sure you have a copy of the manifest utility "mt.exe" in the current folder!
    goto :EOF
)

if x%1==x (
    echo Make script to create executables out of C Code.
    echo.
    echo USAGE: make FILENAME
    echo           - FILENAME without extension
    goto :EOF
)

:start
if exist %1.exe del %1.exe

call %TC_ROOT%\sample\compile -DIPLIB=none %1.cpp
if not %ERRORLEVEL%==0 goto error
echo.
call %TC_ROOT%\sample\linkitk -o %1 %1.obj
if not %ERRORLEVEL%==0 goto error
echo.
mt /manifest %1.exe.manifest /outputresource:%1.exe;#1
if not %ERRORLEVEL%==0 goto error

:cleanup
if exist %1.exe.manifest del %1.exe.manifest
if exist %1.obj del %1.obj
if exist %1.pdb del %1.pdb
if exist %1.ilk del %1.ilk
if exist %1.exp del %1.exp
if exist %1.lib del %1.lib

exit /B %ERRORLEVEL%

:error
echo ERROR: Please check for errors!
goto cleanup

pause