@echo off

set TC_BIN=%TC_ROOT%\bin

for %%f in (*.xml) do call %TC_ROOT%\bin\preferences_manager.exe -u=infodba -p=UGSup5050 -g=dba -mode=import -action=OVERRIDE -scope=GROUP -target="%%~nf" -file="%%~nxf" -report_file="%~dp0\%%~nxf.log"

