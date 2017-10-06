@ECHO off
:: # ------------------------------------------------------------------------------
:: # patches the client with the latest custom jar files
:: # PARAMETER INPUT:
:: # 	NONE
:: # EXPECTED ENVIRONEMNT VARIABLES:
:: # 	NONE
:: # SETENVIRONEMNT VARIABLES (needed by follwo up scripts):
:: # 	NONE	
:: # ------------------------------------------------------------------------------


:: # Set these variables mannually dending on the system 

:: Usally set by the installation in the system envirment variables
:: To use it within stratup Script remove BTC4T_ or change with global settings
setlocal

::Patch Environment
:: check initialization for environment
set PATCH_ENV=%1


:: environments
set BTC4T_PLUGIN_ROOT=C:\ugs\tc\%TC_VERSION%_plugins\%PATCH_ENV%\btportal\eclipse\plugins
set BTC4T_PORTAL_ROOT=C:\ugs\tc\%TC_VERSION%\portal
set PATCH_PATH=%~dp0content
set PATCH_TP=%~dp0..\Tools

:: ensure that the folder exists and is empty
RMDIR %BTC4T_PLUGIN_ROOT% /S /Q
if not exist %BTC4T_PLUGIN_ROOT% mkdir %BTC4T_PLUGIN_ROOT%

:: ensure that saplogon.properties is deleted
if exist %BTC4T_PORTAL_ROOT%\saplogon.properties del /f /q %BTC4T_PORTAL_ROOT%\saplogon.properties

:: # copy to client portal
xcopy %PATCH_PATH% %BTC4T_PLUGIN_ROOT%\* /y /v /q
if not %errorlevel%==0 exit /b 1

xcopy %PATCH_PATH%\com.tcpb.rac.plmxml_tools.external %BTC4T_PLUGIN_ROOT%\com.tcpb.rac.plmxml_tools.external\* /s /e /y /v /q
if not %errorlevel%==0 exit /b 1

xcopy %PATCH_PATH%\T4S_Client_Plugins %BTC4T_PLUGIN_ROOT%\* /y /v /q
if not %errorlevel%==0 exit /b 1


if /i "%PATCH_ENV%" == "prod" (
	
	xcopy %PATCH_PATH%\T4S_PKI_CLIENT_PRODUCTION\*.jar %BTC4T_PLUGIN_ROOT%\* /y /v /q
	if not %errorlevel%==0 exit /b 1

	xcopy %PATCH_PATH%\T4S_PKI_CLIENT_PRODUCTION\*.properties %BTC4T_PORTAL_ROOT%\* /y /v /q
	if not %errorlevel%==0 exit /b 1
)

if /i "%PATCH_ENV%" == "test" (
	
	xcopy %PATCH_PATH%\T4S_PKI_CLIENT_TEST\*.jar %BTC4T_PLUGIN_ROOT%\* /y /v /q
	if not %errorlevel%==0 exit /b 1

	xcopy %PATCH_PATH%\T4S_PKI_CLIENT_TEST\*.properties %BTC4T_PORTAL_ROOT%\* /y /v /q
	if not %errorlevel%==0 exit /b 1
)

::call %PATCH_TP%\genregxml.bat
call %BTC4T_PORTAL_ROOT%\registry\genregxml.bat

: exit
endlocal
exit /b 0












