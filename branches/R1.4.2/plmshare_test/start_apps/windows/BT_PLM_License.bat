@echo off

if /i "%TC_SERVER_MANAGER%" == "true" goto next

:: # SaM - Server-Services can not handle "echo".
if "%SIDT_DEBUG%" == "1" echo on

echo.
echo --------------------------------
echo start source %~n0
echo %0
echo --------------------------------


:next

:: #---------------------------------------------------------------------------------------
:: # Standorte definieren

if not defined STANDORT call %~dp0BT_Site.bat


:: #---------------------------------------------------------------------------------------
:: # Lizenz definieren

:: # Rastatt
if /i "%STANDORT%" == "RAS" (
	set UGS_LICENSE_SERVER=28000@deras900.de-ras.de.abatos.com;28000@chlupi99002srv.ad001.siemens.net;28000@chzrhii9108srv.ad001.siemens.net
	set UGS_LICENSE_BUNDLE=NX11110
)

:: # Zug
if /i "%STANDORT%" == "ZUG" (
	set UGS_LICENSE_SERVER=28000@chlupi99002srv.ad001.siemens.net;28000@chzrhii9108srv.ad001.siemens.net
	set UGS_LICENSE_BUNDLE=
)

:: # Peking
if /i "%STANDORT%" == "PEK" (
	set UGS_LICENSE_SERVER=28000@CNPEK01739.ad001.siemens.net;28000@chlupi99002srv.ad001.siemens.net;28000@chzrhii9108srv.ad001.siemens.net
	set UGS_LICENSE_BUNDLE=
)

:: # Kimpo
if /i "%STANDORT%" == "KPO" (
	set UGS_LICENSE_SERVER=28000@kr001s1052srv.kr001.siemens.net;28000@chlupi99002srv.ad001.siemens.net;28000@chzrhii9108srv.ad001.siemens.net
	set UGS_LICENSE_BUNDLE=ADVDES
)

:: # Buffalo Grove	
if /i "%STANDORT%" == "BFG" (
	set UGS_LICENSE_SERVER=28000@uslzua200kasrv.ww009.siemens.net;28000@chlupi99002srv.ad001.siemens.net;28000@chzrhii9108srv.ad001.siemens.net
	set UGS_LICENSE_BUNDLE=ADVDES
)

set NXN_LICENSE_FILE=%UGS_LICENSE_SERVER%
set SPLM_LICENSE_SERVER=%UGS_LICENSE_SERVER%
set TCVIS_LICENSE_FILE=%UGS_LICENSE_SERVER%

:: # zum testen anderen Lizenzserver eingetragen
rem if /i "%COMPUTERNAME%" == "xxxxxxx" (
rem	set UGS_LICENSE_SERVER=xxxxxxx
rem	set UGS_LICENSE_BUNDLE=ADVDES;P1
rem )

if /i "%TC_SERVER_MANAGER%" == "true" goto next
echo.
echo --------------------------------
echo end source %~n0
echo --------------------------------
echo.
:next