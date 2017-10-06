@echo off


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

echo %~dp0..\..\nx100_%BT_ENV%_plmshare.cmd
call %~dp0..\..\nx100_%BT_ENV%_plmshare.cmd

call %SPLM_SHR_DIR%\start_apps\windows\start_%NX_VERSION%.bat %SIDT_LANG% %SIDT_APPL%


exit
