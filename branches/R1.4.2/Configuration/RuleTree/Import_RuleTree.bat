@echo off


pushd "%~dp0"

SET TC_ROOT=N:\ugs\tc11\11
SET TC_DATA=N:\ugs\tc11\tcdata
call %TC_DATA%\tc_profilevars.bat

echo.
echo -----------------------------------------------------------------------
echo IMPORTING Access Rule Tree
echo -----------------------------------------------------------------------
echo. 
mkdir .\logs\RuleTree

rem FOR %%i IN ("RuleTree\*.xml") DO (
FOR /F "tokens=*" %%i IN ('dir /b /s /a-d /OD .\*.xml') DO (
	REM n=File Name, x=File Extensions
	echo plmxml_import -u=infodba -p=%DBAPASS_TMP% -g=dba -xml_file="%%i" -log=".\logs\RuleTree\%%~ni.log"
	plmxml_import -u=infodba -p=%DBAPASS% -g=dba -xml_file="%%i" -log=".\logs\RuleTree\%%~ni.log"
)