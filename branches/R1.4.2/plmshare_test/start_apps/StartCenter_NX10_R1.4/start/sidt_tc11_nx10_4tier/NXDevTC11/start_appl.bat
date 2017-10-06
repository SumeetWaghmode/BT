@echo off


:: # ------------------------------------------------------------------------------
:: # Environment BT
:: # ------------------------------------------------------------------------------

set ORACLE_SID_TMP=%1
set BT_PLM_SITE=%2

set TC_VERS=tc112
set NX_VERSION=nx100
set BT_ENV=NXDevTC11

set SIDT_APPL=portal_client
set SIDT_LANG=en
rem default set SIDT_LANG=en


call %~dp0..\..\nx100_%BT_ENV%_plmshare.cmd

rem set BT_TC_ROOT_EXTENSION=1PDM4BT

call %SPLM_SHR_DIR%\start_apps\windows\start_%NX_VERSION%.bat %SIDT_LANG% %SIDT_APPL% %ORACLE_SID% %TC_VERS%


REM exit
