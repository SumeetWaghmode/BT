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


:: TODO: in case there are additional systems these has to be added here
setlocal

if /i "%PATCH_ENV%" == "prod" goto next

if not exist C:\ugs\tc\%TC_VERSION%_plugins\*.txt goto next
	del C:\ugs\tc\%TC_VERSION%_plugins\*.txt
:next

:: define latest patch here
set LATEST_PATCH_RELEASE=CPR_1PDM4BT_142

:: check initialization for environment
set PATCH_ENV=%1
if not defined PATCH_ENV goto patch_env_error

:: environments
set BTC4T_PLUGIN_FOLDER=C:\ugs\tc\%TC_VERSION%_plugins\

:: check if ugrade is already done
if exist %BTC4T_PLUGIN_FOLDER%\%LATEST_PATCH_RELEASE%.txt goto exit

:: chek if release notes are existing
if not exist %~dp0%LATEST_PATCH_RELEASE%\%LATEST_PATCH_RELEASE%.txt goto patch_txt_error

:: in case not execute the patch
echo patching client to %LATEST_PATCH_RELEASE%
call %~dp0%LATEST_PATCH_RELEASE%\%LATEST_PATCH_RELEASE%.bat %PATCH_ENV%

if %errorlevel%==0 (
	copy /Y /V %~dp0%LATEST_PATCH_RELEASE%\%LATEST_PATCH_RELEASE%.txt %BTC4T_PLUGIN_FOLDER%
	goto exit
)

goto patch_error

:patch_env_error
echo -----------------------------------------
echo ERROR Patch environement int, test or prod
echo is not set
echo Please call support for help
echo -----------------------------------------
pause
goto exit


:patch_txt_error
echo -----------------------------------------
echo ERROR Patch release notes are missing
echo Please call support for help
echo -----------------------------------------
pause
goto exit

:patch_error
echo -----------------------------------------
echo ERROR %LATEST_PATCH_RELEASE% exits with errorlevel : %errorlevel%
echo Please call support for help
echo -----------------------------------------
pause
goto exit

:exit
endlocal








