@echo off

set THIS_DIR=%~dp0
cd /d %THIS_DIR%

REM Set Teamcenter Environment
SET TC_ROOT=%TC_ROOT%
SET TC_DATA=%TC_DATA%
call %TC_DATA%\tc_profilevars.bat

REM Configuration
SET USR=infodba
SET PWD=UGSup5050
SET GRP=dba
SET EXPORT_HISTORY=%THIS_DIR%config\export-history.xml
SET DOCUMENT_LIST=%THIS_DIR%config\documentlist.xml
SET ROOT_EXPORT_DIR=%THIS_DIR%PLM_Dokumente

call bt7_report_pdbrowser.exe -u=%USR% -p=%PWD% -g=%GRP% -export_history=%EXPORT_HISTORY% -document_list=%DOCUMENT_LIST% -root_export_dir=%ROOT_EXPORT_DIR% -mode=full

exit /b %ERRORLEVEL%