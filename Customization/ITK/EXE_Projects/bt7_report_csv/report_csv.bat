@echo off

set THIS_DIR=%~dp0
cd /d %THIS_DIR%

REM Set Teamcenter Environment
SET TC_ROOT=N:\ugs\tc11\11
SET TC_DATA=N:\ugs\tc11\tcdata
call %TC_DATA%\tc_profilevars.bat

REM Configuration
SET USR=infodba
SET PWD=UGSup5050
SET GRP=dba

SET REPORTTYPE=%1
SET ECADCONFIG=%2
SET OUTPUT=%3

REM Generate Report
call bt7_report_csv.exe -u=%USR% -p=%PWD% -g=dba -report_type=%REPORTTYPE% -ecad_config=%ECADCONFIG% -output=%OUTPUT%

exit /b %ERRORLEVEL%