@echo off

REM Set Teamcenter-Env
SET TC_ROOT=C:\Siemens\Teamcenter8
SET TC_DATA=C:\Siemens\tcdata
call %TC_DATA%\tc_profilevars

REM Get current dir
set THIS_DIR=%~dp0
set THIS_DIR=%THIS_DIR:~0,-1%
set THIS_DIR="%THIS_DIR%"

REM Init
set BT_CODE_DIR=C:\$SBT_Repository\trunk\Customization\ITK\SBT_Extensions
set MSDEV_HOME=C:\Program Files (x86)\Microsoft Visual Studio 8\VC
call "%MSDEV_HOME%\vcvarsall.bat" x86_amd64

REM Copy all files to the current folder
cd /d %BT_CODE_DIR%
copy *.c %THIS_DIR%
copy *.cpp %THIS_DIR%
copy *.h %THIS_DIR%
copy *.hpp %THIS_DIR%

cd /d %THIS_DIR%

REM Loop over all source-files(*.c, *.cpp, *.cxx) and compile them
for /r %%i in (*.c*) do call %TC_ROOT%\sample\compile -64bit -DIPLIB=none %%i

cd /d %THIS_DIR%

REM Linking     
call %TC_ROOT%\sample\link_custom_exits libBT7_Extensions

REM Delete all files from the current folder (keep *.dll, *.lib, *.pdb)
del *.cxx
del *.cpp
del *.hxx
del *.hpp
del *.h
del *.obj
del *.exp
del *.ilk
del *.manifest