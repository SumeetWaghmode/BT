@echo on


:: # ------------------------------------------------------------------------------
:: # Environment BT
:: # ------------------------------------------------------------------------------

set SIDT_APPL=%1
set BT_PLM_SITE=%2
set ORACLE_SID=

set TC_VERS=tc11
set NX_VERSION=nx100
set BT_ENV=prod

set SIDT_LANG=en

rem CF, 5.6.14
rem Bestimmung des SIDT_PARAM3. Für das Testsystem immer die Test-SIDT
if /i "%BT_PLM_SITE%" == "RAS" (
	set ORACLE_SID=tcuacsp
	)
if /i "%BT_PLM_SITE%" == "ZUG" (
	set ORACLE_SID=tcuacsp
	)
if /i "%BT_PLM_SITE%" == "PEK" (
	set ORACLE_SID=tcengpkp
	)
if /i "%BT_PLM_SITE%" == "KPO" (
	set ORACLE_SID=tcengkpp
	)
if /i "%BT_PLM_SITE%" == "BFG" (
	set ORACLE_SID=tcengbgp
	)

call %~dp0..\..\%NX_VERSION%_%BT_ENV%_plmshare.cmd

call %SPLM_SHR_DIR%\start_apps\windows\start_%NX_VERSION%.bat %SIDT_LANG% %SIDT_APPL% %ORACLE_SID%


exit
