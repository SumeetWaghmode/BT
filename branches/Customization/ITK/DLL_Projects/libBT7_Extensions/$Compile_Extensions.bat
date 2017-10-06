@echo on

REM Set Teamcenter-Env
SET TC_ROOT=N:\ugs\tc11\11
SET TC_DATA=N:\ugs\tc11\tcdata
call %TC_DATA%\tc_profilevars

REM Get current dir
set THIS_DIR=%~dp0
set THIS_DIR=%THIS_DIR:~0,-1%
set THIS_DIR="%THIS_DIR%"

REM Init
REM set BT_CODE_DIR=C:\$SBT_Repository\trunk\Customization\Libnorm64\source_64\New folder
set MSDEV_HOME=C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC
call "%MSDEV_HOME%\vcvarsall.bat" x86_amd64

REM Copy all files to the current folder
REM cd /d %BT_CODE_DIR%
REM copy *.c %THIS_DIR%
REM copy *.cpp %THIS_DIR%
REM copy *.h %THIS_DIR%
REM copy *.hpp %THIS_DIR%

cd /d %THIS_DIR%

REM Loop over all source-files(*.c, *.cpp, *.cxx) and compile them
for /r %%i in (*.c*) do call %TC_ROOT%\sample\compile -DIPLIB=none %%i

cd /d %THIS_DIR%

REM Linking     
call %TC_ROOT%\sample\link_custom_exits libBT7_Extensions

REM Delete all files from the current folder (keep *.dll, *.lib, *.pdb)
del *.obj
del *.exp
del *.ilk
del *.manifest