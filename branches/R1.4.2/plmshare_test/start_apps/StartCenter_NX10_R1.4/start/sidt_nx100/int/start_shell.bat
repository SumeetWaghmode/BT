@echo off


:: # ------------------------------------------------------------------------------
:: # Environment BT
:: # ------------------------------------------------------------------------------

set SIDT_APPL=%1
set BT_PLM_SITE=%2

set TC_VERS=tc83
set NX_VERSION=nx100
set BT_ENV=int

set SIDT_APPL=nx_prompt
set SIDT_LANG=en

rem CF, 5.6.14
rem Bestimmung des SIDT_PARAM3. Für das Testsystem immer die Test-SIDT
if /i "%BT_PLM_SITE%" == "RAS" (
	set ORACLE_SID=tcuacst
	)
if /i "%BT_PLM_SITE%" == "ZUG" (
	set ORACLE_SID=tcuacst
	)
if /i "%BT_PLM_SITE%" == "PEK" (
	set ORACLE_SID=tcengpkt
	)
if /i "%BT_PLM_SITE%" == "KPO" (
	set ORACLE_SID=tcengkpt
	)
if /i "%BT_PLM_SITE%" == "BFG" (
	set ORACLE_SID=tcengbgt
	)

call %~dp0..\..\%NX_VERSION%_%BT_ENV%_plmshare.cmd


call %SPLM_SHR_DIR%\start_apps\windows\start_%NX_VERSION%.bat %SIDT_LANG% %SIDT_APPL% %ORACLE_SID%
set PATH=%UGII_ROOT_DIR%;%PATH%
cmd

exit
