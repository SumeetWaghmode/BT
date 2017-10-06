setlocal

title Teamcenter Variant Manager

set CFG_FILE=%~dp0config.ini
set CFG_FILE_CUS=%~dp0config_cust.ini
set CFG_FILE_LOC=%~dp0config_%COMPUTERNAME%.ini

set JAVA_PROPS=-Djava.library.path="%FMS_HOME%/lib"
::set JAVA_PROPS=%JAVA_PROPS% -Djavax.net.ssl.trustStore="%~dp0truststore.jks" -Djavax.net.ssl.trustStorePassword=changeit

set CLASSPATH=%~dp0;%~dp0*;%~dp0lib\*;%~dp0lib\11\*;%~dp0lib\10\*

@if exist "%CFG_FILE%" for /F "usebackq eol=# delims=" %%l in ("%CFG_FILE%") do @call set %%l
@if exist "%CFG_FILE_CUS%" for /F "usebackq eol=# delims=" %%l in ("%CFG_FILE_CUS%") do @call set %%l
@if exist "%CFG_FILE_LOC%" for /F "usebackq eol=# delims=" %%l in ("%CFG_FILE_LOC%") do @call set %%l

if not defined vm_WorkDir set vm_WorkDir=%vm_IniWorkDir%
if not defined vm_WorkDir set vm_WorkDir=%TEMP%

if not defined vm_LogFile set vm_LogFile=%vm_WorkDir%\varman.log
if not defined vm_ErrFile set vm_ErrFile=%vm_WorkDir%\varman.err

call :ENSURE_PATH %vm_ErrFile%
del "%vm_LogFile%" %vm_ErrFile% 2>nul

@set PAR=%*

if defined PAR goto :CALL_TC_ADAPTER

set PAR=-h=%vm_TCUrl% -u=%vm_TCUsr% -p=%vm_TCPwd% -g=%vm_TCGrp%
set vm_ErrFile=^&1

echo OFF

cls

echo.
echo 1 - run tc import
echo 2 - run tc export
echo 3 - run tc clone
echo 4 - run tc batch clone
echo 0 - manual mode
echo.
set /p v=(Input 1,2,3,4 or 0):

if "%v%" == "1" set PAR=%PAR% -scr=tc_import.s#
if "%v%" == "2" set PAR=%PAR% -scr=tc_export.s#
if "%v%" == "3" set PAR=%PAR% -scr=tc_clone.s#
if "%v%" == "4" set PAR=%PAR% -scr=tc_clone.s# -mode=BATCH

echo ON

set PAR=%PAR% -item="%vm_TCItem%"
set PAR=%PAR% -ptk="%vm_PTKItem%"
set PAR=%PAR% -asn="%vm_ASN%"
set PAR=%PAR% -var_rule="%vm_ASN%"
set PAR=%PAR% -out="%vm_WorkDir%\import.txt"
set PAR=%PAR% -exp="@%vm_WorkDir%\export.txt"

:CALL_TC_ADAPTER

call :ENSURE_JAVA

java -version
if not %ERRORLEVEL% == 0 (
	echo JAVA not found >%vm_ErrFile%
) else (
	(java %JAVA_PROPS% com.drweco.vm.tc_adapter.uface.Main %PAR% || echo RETURN_CODE = -1 1>&2) 2>%vm_ErrFile% | "%~dp0wtee.exe" "%vm_LogFile%"
)

if "%vm_ErrFile%" == "&1" goto :NOERRF

set /P err=<%vm_ErrFile%
if not defined err goto :NOERRF

set RETURN_CODE=-1
@type %vm_ErrFile% >> "%vm_LogFile%"

:NOERRF
@type %vm_ErrFile%

if defined v (
  pause
  exit /b %RETURN_CODE%
) else (
  exit %RETURN_CODE%
)

:ENSURE_PATH
mkdir %~dp1 2>nul

:ENSURE_JAVA

where java
if %ERRORLEVEL% == 0 exit /b

set JRE=
for /f %%d in ('dir /b /ad "%ProgramFiles%\Java\jre*"') do set JRE=%ProgramFiles%\Java\%%d\bin;

set PATH=%JRE%%PATH%

where java
if %ERRORLEVEL% == 0 exit /b

for /f %%d in ('dir /b /ad "%ProgramFiles(x86)%\Java\jre*"') do set JRE=%ProgramFiles(x86)%\Java\%%d\bin;

set PATH=%JRE%%PATH%

exit /b

endlocal
