@echo off

set pw=UGSup5050

set currentDir=%cd%
cd /d "%currentDir%"
echo "%currentDir%"
pause

rem for %%i IN ("%currentDir%\*.xml") DO %TC_BIN%\import_file -u=infodba -p=%pw% -g=dba -f="%%i" -d="%%~ni" -type=XMLRenderingStylesheet -ref=XMLRendering -de=r -vb -log="%%~ni.log" & echo %TC_BIN%\import_file -u=infodba -p=%pw% -g=dba -f="%%i" -d="%%~ni" -type=XMLRenderingStylesheet -ref=XMLRendering -de=r -vb -log="%%~ni.log"
for %%i IN ("%currentDir%\*.xml") DO %TC_BIN%\import_file -u=infodba -p=%pw% -g=dba -f="%%i" -d="%%~ni" -type=XMLRenderingStylesheet -ref=XMLRendering -de=r -vb -log="%%~ni.log"
echo.
echo import finish xml
echo.

echo.
%TC_BIN%\preferences_manager -u=infodba -p=%pw% -g=dba -mode=import -file="%currentDir%\Tools\preferences\SiteStylesheetPreferences.xml" -scope=SITE -action=OVERRIDE -report_file=import.log
echo.
echo import finish preference
echo.
