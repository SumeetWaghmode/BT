@echo off


:: # ------------------------------------------------------------------------------
:: # switch_4tier_server.bat sitchs the 4tier_server_URLS
:: # It is called by the startup manager application on pressing the button
:: # Works with local variables as int is only existing in ZUG
:: # PARAMETER INPUT:
:: # 	CCFG_SITE 	... The SITE (ZUG, RAS, PEK, KPO, BFG, MRT)
:: # 	CCFG_INT 	... The Environement (PROD, int, PLMCint)
:: #	CCFG_DEBUG 	... TEST (shows but not executes) ON (SHOWS DEBUG Informations) or OFF
:: # EXPECTED ENVIRONMNT VARIABLES:
:: # 	NONE
:: # SETENVIRONEMNT VARIABLES (needed by follwo up scripts):
:: # 	NONE	
:: # ------------------------------------------------------------------------------


:: # Set these variables mannually dending on the system 

:: Usally set by the installation in the system envirment variables
:: To use it within stratup Script remove BTC4T_ or change with global settings


setlocal


:: Initialize handover parameter

set CCFG_SITE=%1
set CCFG_ENV=%2
set CCFG_DEBUG=%3

::echo ------------------------------------------------
::echo stop at %~n0 %CCFG_SITE% %CCFG_ENV% %CCFG_DEBUG%
::echo ------------------------------------------------
::pause

:: check initialization 
if not defined CCFG_SITE goto invalid_parameters
if not defined CCFG_ENV goto invalid_parameters
if not defined CCFG_DEBUG set CCFG_DEBUG=OFF

:: DEBUG: shows Parameters
if /i not "%CCFG_DEBUG%" == "OFF" (
	echo SITE: %CCFG_SITE%
	echo ENV: %CCFG_ENV%
	echo DEBUG: %CCFG_DEBUG%
)

::-------------------------------------------------------------------------
:: Do the switch according to the input parameters
:: PLMCare Integration environment
::if /i "%CCFG_ENV%" == "plmcint" (
::	if /i "%CCFG_SITE%" == "MRT" (
::		if /i "%CCFG_DEBUG%" == "TEST" (
::			echo  TEST: call %~dp0switch_4tier_server_plmcint.bat %CCFG_DEBUG%
::			pause
::		)
::		call %~dp0switch_4tier_server_plmcint.bat %CCFG_DEBUG%
::		goto end
::	)
::)

:: BT Integration (All Sites)
::-------------------------------------------------------------------------
::if /i "%CCFG_ENV%" == "INT" (
::	if /i "%CCFG_SITE%" == "MRT" (
::		goto invalid_parameters
::	)
::	if /i "%CCFG_DEBUG%" == "TEST" (
::		echo  TEST: %~dp0switch_4tier_server_int.bat %CCFG_DEBUG%
::		pause
::	)
::	call %~dp0switch_4tier_server_%CCFG_ENV%.bat %CCFG_DEBUG%
::	goto end
::)

:: BT TEST
::if /i "%CCFG_ENV%" == "TEST" (
::	if /i "%CCFG_DEBUG%" == "TEST" (
::			echo  TEST: %~dp0switch_4tier_server_test.bat %CCFG_DEBUG%
::			pause
::		)
::		call %~dp0switch_4tier_server_%CCFG_ENV%.bat %CCFG_DEBUG%
::		goto end
::	)
::)
::-------------------------------------------------------------------------

if /i "%CCFG_DEBUG%" == "TEST" (
	echo  TEST: %~dp0switch_4tier_server_int.bat %CCFG_DEBUG%
	pause
)
	call %~dp0switch_4tier_server_%CCFG_ENV%.bat %CCFG_DEBUG%
	goto end


:: BT Produktion
if /i "%CCFG_ENV%" == "PROD" (
	if /i "%CCFG_SITE%" == "ZUG" (
		if /i "%CCFG_DEBUG%" == "TEST" (
			echo  TEST: %~dp0switch_4tier_server_prod.bat %CCFG_DEBUG%
			pause
		)
		call %~dp0switch_4tier_server_prod.bat %CCFG_DEBUG%
		goto end
	)
	if /i "%CCFG_SITE%" == "RAS" (
		if /i "%CCFG_DEBUG%" == "TEST" (
			echo  TEST: %~dp0switch_4tier_server_prod_ras.bat %CCFG_DEBUG%
			pause
		)
		call %~dp0switch_4tier_server_prod_ras.bat %CCFG_DEBUG%
		goto end
	)
	if /i "%CCFG_SITE%" == "PEK" (
		if /i "%CCFG_DEBUG%" == "TEST" (
			echo  TEST: %~dp0switch_4tier_server_prod_pek.bat %CCFG_DEBUG%
			pause
		)
		call %~dp0switch_4tier_server_prod_pek.bat %CCFG_DEBUG%
		goto end
	)
	if /i "%CCFG_SITE%" == "BFG" (
		if /i "%CCFG_DEBUG%" == "TEST" (
			echo  TEST: %~dp0switch_4tier_server_prod_bfg.bat %CCFG_DEBUG%
			pause
		)
		call %~dp0switch_4tier_server_prod_bfg.bat %CCFG_DEBUG%
		goto end
	)
	if /i "%CCFG_SITE%" == "KPO" (
		if /i "%CCFG_DEBUG%" == "TEST" (
			echo  TEST: %~dp0switch_4tier_server_prod_kpo.bat %CCFG_DEBUG%
			pause
		)
		call %~dp0switch_4tier_server_prod_kpo.bat %CCFG_DEBUG%
		goto end
	)
)


pause
goto invalid_parameters

:invalid_parameters
echo -----------------------------------------
echo ERROR Invalid parameters or clietconfig not existing
echo CCFG_SITE: ZUG, RAS, PEK, KPO, BFG, MRT - current value CCFG_SITE=%CCFG_SITE%
echo CCFG_ENV: prod, int, plmcint expected - current value CCFG_ENV=%CCFG_ENV%
echo -----------------------------------------

pause
goto exit

:end
echo -----------------------------------------
echo Client configuration changed to: SITE: %CCFG_SITE% INSTANCE: %CCFG_ENV%
echo -----------------------------------------

:exit
endlocal





