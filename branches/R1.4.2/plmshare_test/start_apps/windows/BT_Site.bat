@echo off

echo.
echo --------------------------------
echo start source %~n0
echo %0
echo --------------------------------
echo.

:: # Last modified by:		Samuel Meier
:: # Last modification:		Changing to LOGONSERVER, make installation identify sites.
:: # Date last modified:	24.04.2014
:: # CF, 3.12.13. compute site based on cliente name for site Rastatt, as logonserver leads for Rastatt to erroneous results

if /i "%TC_SERVER_MANAGER%" == "true" goto next

if "%SIDT_DEBUG%" == "1" echo on

echo.
echo --------------------------------
echo start source %~n0
echo %0
echo --------------------------------


:next


:: #---------------------------------------------------------------------------------------
:: # Standorte definieren
set LOGONSERVER_SITE=%LOGONSERVER:~2,2%
:: # 19.11.13, CF: Variablen wieder hinzugefügt, weil darueber das Verzeichnis mit der User-Konfiguration gesteuert wird
set COMPUTERNAME_SHORT1=%COMPUTERNAME:~0,4%
set COMPUTERNAME_SHORT2=%COMPUTERNAME:~0,3%
set COMPUTERNAME_SHORT3=%COMPUTERNAME:~0,2%

set STANDORT=XX

if defined BT_PLM_SITE (
	set STANDORT=%BT_PLM_SITE%
	goto next
)

:: # 19.11.13 Logonserver ZUG \\CHZUGZW9A01ADS
if /i "%LOGONSERVER_SITE%" == "CH"           set STANDORT=ZUG

:: # 19.11.13 Logonserver PEK \\PEKCG01A
if /i "%LOGONSERVER_SITE%" == "PE"           set STANDORT=PEK

:: # 29.09.14 Logonserver PEK \\CNPEK05001
if /i "%LOGONSERVER_SITE%" == "CN"           set STANDORT=PEK

:: # 19.11.13 Logonserver KPO \\KR001S1002ADS
if /i "%LOGONSERVER_SITE%" == "KR"		     set STANDORT=KPO

:: # 19.11.13 Logonserver BFG \\USLZUA20004ADS
if /i "%LOGONSERVER_SITE%" == "US"		     set STANDORT=BFG

:: # 19.11.13 Logonserver RAS \\DERAS021102
:: # if /i "%LOGONSERVER_SITE%" == "DE"        set STANDORT=RAS
if /i "%COMPUTERNAME_SHORT3%" == "de"        set STANDORT=RAS


:next
:: # 13.9.16, CF: Applicationserver eintragen: Problem mit der Auswertung des Standorts beim NetMgr Start
if /i "%COMPUTERNAME%" == "CHZRHII900jsrv"   set STANDORT=ZUG&&set BT_PLM_SITE=ZUG



:: # 13.9.16, CF: Renderserver eintragen
if /i "%COMPUTERNAME%" == "chzrhii9109srv"	 set STANDORT=ZUG&&set BT_PLM_SITE=ZUG	

:: # No STANDORT
if /i "%STANDORT%" == "XX" (
goto no_site
)


:: #---------------------------------------------------------------------------------------
:: # Site-specific Server-Settings (CF)
:: # BT_CRI_SHR is used for Render-Server
:: #-----------------------------------------------
:: # Rastatt - Clients on site and local Servers
:: # CF changed from (if /i "%SIDT_PAR3%" ==)   to (if /i "%BT_ENV%" ==) 
:: # CF: Prod, Int system share the same ORACLE_SID, but differ in BT_ENV
if /i "%STANDORT%" == "RAS" (
rem 	if /i "%BT_ENV%" == "tcuacst" (
rem 		set BT_UG_SERVER=ch1w80166.ad001.siemens.net
rem 		set BT_CRI_SHR=\\chzrhii901bsrv.ww002.siemens.net\plmshare
rem 	)
	if /i "%BT_ENV%" == "PROD" (
		set BT_UG_SERVER=deras900.de-ras.de.abatos.com
	)
	if /i "%BT_ENV%" == "TEST" (
		set BT_UG_SERVER=chlupi99012srv.ad001.siemens.net
	)
	if /i "%BT_ENV%" == "INT" (
		set BT_UG_SERVER=185.48.117.104
	)
	if /i "%BT_ENV%" == "" (
		set BT_UG_SERVER=ch1w80166.ad001.siemens.net
    )
)

:: renderserver 
if /i "%COMPUTERNAME%" == "chzrhii900msrv"   set BT_UG_SERVER=chzrhii900nsrv.ww002.siemens.net


:: #-----------------------------------------------
:: # Zug - Clients on Site and Servers in DC Zurich
if /i "%STANDORT%" == "ZUG" (
	if /i "%BT_ENV%" == "PROD" (
		set BT_UG_SERVER=chlupi99003srv.ad001.siemens.net
	)
	if /i "%BT_ENV%" == "TEST" (
		set BT_UG_SERVER=chlupi99012srv.ad001.siemens.net
	)
	if /i "%BT_ENV%" == "INT" (
		set BT_UG_SERVER=185.48.117.104
	)
	if /i "%BT_ENV%" == "" (
		set BT_UG_SERVER=ch1w80166.ad001.siemens.net
	)
)

:: #-----------------------------------------------
:: # Peking
if /i "%STANDORT%" == "PEK" (
	if /i "%BT_ENV%" == "PROD" (
		set BT_UG_SERVER=CNPEK01343.ad001.siemens.net
	)
	if /i "%BT_ENV%" == "TEST" (
		set BT_UG_SERVER=CNPEK01344.ad001.siemens.net
	)
	if /i "%BT_ENV%" == "" (
		set BT_UG_SERVER=CNPEK01344.ad001.siemens.net
	)	
)

:: #-----------------------------------------------
:: # Kimpo
if /i "%STANDORT%" == "KPO" (
	if /i "%BT_ENV%" == "prod" (
		set BT_UG_SERVER=kr001s1056srv.ad001.siemens.net
	)
	if /i "%BT_ENV%" == "TEST" (
		set BT_UG_SERVER=kr001s1056srv.ad001.siemens.net
	)
	if /i "%BT_ENV%" == "" (
		set BT_UG_SERVER=kr001s1056srv.ad001.siemens.net
	)
)

:: #-----------------------------------------------
:: # Buffalo Grove	
if /i "%STANDORT%" == "BFG" (
	if /i "%BT_ENV%" == "PROD" (
		set BT_UG_SERVER=uslzua200kpsrv.ad001.siemens.net
	)
	if /i "%BT_ENV%" == "TEST" (
		set BT_UG_SERVER=uslzua200kpsrv.ad001.siemens.net
	)
	if /i "%BT_ENV%" == "" (
		set BT_UG_SERVER=uslzua200kpsrv.ad001.siemens.net
	)	
)


:: #---------------------------------------------------------------------------------------
:: # set Webserver
set BT_HELP_WEB_PROTOKOLL=http
set BT_HELP_WEB_SERVER=chlupi99007srv.ad001.siemens.net

:: #---------------------------------------------------------------------------------------
:: # set DB specific Web variables
if /i "%ORACLE_SID%" == "tcuacst testsystem" (
	set BT_TC_WEB_SERVER=tcuacst.bt.siemens.com
	set BT_TC_WEB_PORT=44447
	set BT_TC_WEB_PROTOKOLL=https
)
if /i "%ORACLE_SID%" == "tcuacsp" (
	set BT_TC_WEB_SERVER=chlupi99007srv.ad001.siemens.net
	set BT_TC_WEB_PORT=51000
	set BT_TC_WEB_PROTOKOLL=http
)
if /i "%ORACLE_SID%" == "NXDevTC11" (
	set BT_TC_WEB_SERVER=MRT-SBT-INT
	set BT_TC_WEB_PORT=44445
	set BT_TC_WEB_PROTOKOLL=https
)
if /i "%ORACLE_SID%" == "tcuacst" (
	set BT_TC_WEB_SERVER=chlupi99014srv.ad001.siemens.net
	set BT_TC_WEB_PORT=51000
	set BT_TC_WEB_PROTOKOLL=http
)


:: #---------------------------------------------------------------------------------------
:: # set PLM-Share
::set UGS_SHR_DIR=\\%BT_UG_SERVER%\plmshare112
::set SPLM_SHR_DIR=\\%BT_UG_SERVER%\plmshare
if /i "%BT_ENV%" == "PROD" (
	set UGS_SHR_DIR=\\%BT_UG_SERVER%\plmshare112
	set SPLM_SHR_DIR=\\%BT_UG_SERVER%\plmshare112
)
rem cf: für den start der esx umgebung ist für int der alte plmshare namen eingetragen
rem cf: anpassen auf plmshare112_test
if /i "%BT_ENV%" == "TEST" (
	set UGS_SHR_DIR=\\%BT_UG_SERVER%\plmshare112
	set SPLM_SHR_DIR=\\%BT_UG_SERVER%\plmshare112
)

::echo at %~n0 %STANDORT% %BT_ENV% %TC_VERS% %SPLM_SHR_DIR%
::pause

:: # for PLM-Share Servers only
if /i "%COMPUTERNAME%" == "CNPEK01343"     set UGS_SHR_DIR=O:\plmshare_test
if /i "%COMPUTERNAME%" == "CNPEK01343"     set SPLM_SHR_DIR=O:\plmshare_test
if /i "%COMPUTERNAME%" == "kr001s1052srv"  set UGS_SHR_DIR=O:\plmshare_test
if /i "%COMPUTERNAME%" == "kr001s1052srv"  set SPLM_SHR_DIR=O:\plmshare_test
if /i "%COMPUTERNAME%" == "uslzua200kpsrv" set UGS_SHR_DIR=P:\plmshare_test
if /i "%COMPUTERNAME%" == "uslzua200kpsrv" set SPLM_SHR_DIR=P:\plmshare_test

goto end

:no_site
echo ________________________________________________________________________________
echo.
echo Error: Your TC-Site can not be identified. Please contact your Administrator 
echo for Teamcenter!
echo.
echo STANDORT=%STANDORT%
echo ________________________________________________________________________________
echo.
pause

:end
if /i "%TC_SERVER_MANAGER%" == "true" goto next
echo.
echo --------------------------------
echo end source %~n0
echo --------------------------------
echo.
:next


echo.
echo --------------------------------
echo end source %~n0
echo --------------------------------
echo.


