@echo off

set pw=UGSup5050


set currentDir=%cd%
cd /d "%currentDir%"
echo "%currentDir%"
pause
call "%currentDir%\Tools\EnvSetup.bat" >nul


echo.
REM plmxml_import -u=infodba -p=cloneinfodba -g=dba -xml_file="BT Change Order - Sub Process.xml" -transfermode=workflow_template_overwrite

for %%i IN ("%currentDir%\*.xml") DO "%TC_ROOT%\bin\plmxml_import.exe" -u=infodba -p=%pw% -g=dba -xml_file="%%i" -transfermode=workflow_template_overwrite -log="%%i.txt"
echo.
rem >nul 2>&1 & echo next
echo.
echo import finish
echo.

pause