@echo off

REM Generate Report
call Doc_Rev_LOV_Migration.exe  -t=Doc_Rev_LOV_Migration_Central

exit /b %ERRORLEVEL%