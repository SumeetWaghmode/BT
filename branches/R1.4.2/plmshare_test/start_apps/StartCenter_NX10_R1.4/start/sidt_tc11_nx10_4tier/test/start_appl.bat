@echo off


:: # ------------------------------------------------------------------------------
:: # Environment BT
:: # ------------------------------------------------------------------------------

set ORACLE_SID_TMP=%1
set BT_PLM_SITE=%2

set TC_VERS=tc112
set NX_VERSION=nx100
set BT_ENV=test

set SIDT_APPL=portal_client
set SIDT_LANG=en

echo %~dp0..\..\nx100_%BT_ENV%_plmshare.cmd
call %~dp0..\..\nx100_%BT_ENV%_plmshare.cmd

call %SPLM_SHR_DIR%\start_apps\windows\start_%NX_VERSION%.bat %SIDT_LANG% %SIDT_APPL% %ORACLE_SID% %TC_VERS%

exit
