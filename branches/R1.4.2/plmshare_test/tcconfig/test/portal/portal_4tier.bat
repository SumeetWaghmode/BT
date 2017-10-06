@echo off
:: #-------------------------------------------------------------------
:: # DRWE Anpassungen
echo.
echo --------------------------------
echo start source %~n0 portalbat
echo --------------------------------
echo.

if not defined UGS_APPL_DIR        set UGS_APPL_DIR=c:\ugs
if not defined TC_ROOT             set TC_ROOT=%UGS_APPL_DIR%\tc\1123
if not defined OS_SYSTEM           set OS_SYSTEM=windows
if not defined NX_PLATFORM         set NX_PLATFORM=win64
if not defined OS_VERSION          set OS_VERSION=win
if not defined TC_TMP_DIR          set TC_TMP_DIR=C:\tempugs\temp\tc
if not defined PORTAL_LANGUAGE     set PORTAL_LANGUAGE=en_US

if not exist %TC_TMP_DIR% mkdir %TC_TMP_DIR%

setlocal

rem TPR is short for TC_PORTAL_ROOT to reduce command line length
title Teamcenter Rich Client
set TPR=%TC_ROOT%\portal

rem CF, 4.7.16: lokales Java directory in Teamcenter
rem call "C:\ugs\tc11_r14\install\tem_init.bat"
set TC_JRE_HOME=%UGS_APPL_DIR%\java\jre1.8.0

:: set TPR=C:\ugs\tc\1123\portal
set FMS_HOME=%TC_ROOT%\tccs
if not defined FMS_HOME set FMS_HOME=%TC_ROOT%\tccs


rem use AUX_PATH env var for any additional required paths
rem save original path for external applications
set ORIGINAL_PATH=%PATH%
rem for optimal startup performance, keep the PATH length at a minimum
set PATH=%SYSTEMROOT%\system32;%FMS_HOME%\bin;%FMS_HOME%\lib;%TPR%;%AUX_PATH%

set JAVA_HOME=%TC_JRE_HOME%
set JRE_HOME=%TC_JRE_HOME%

:: # copy keystore file
rem echo copy keystore file...
rem copy /y %DRWE_TC_CONFIG_DIR%\ssl\cacerts %JAVA_HOME%\lib\security\

:start_portal
cd /d %TPR%
set CLASSPATH=.;
set VM_XMX=1024m


@echo Starting Teamcenter Rich Client...
start Teamcenter.exe %* -vm "%JRE_HOME%\bin\javaw.exe" -vmargs -Xmx%VM_XMX% -XX:MaxPermSize=128m -Xbootclasspath/a:"%JRE_HOME%\lib\plugin.jar";"%JRE_HOME%\lib\deploy.jar";"%JRE_HOME%\lib\javaws.jar" -DTCPB_LOCAL=%TCPB_LOCAL% -DTC_TMP_DIR=%TC_TMP_DIR%
