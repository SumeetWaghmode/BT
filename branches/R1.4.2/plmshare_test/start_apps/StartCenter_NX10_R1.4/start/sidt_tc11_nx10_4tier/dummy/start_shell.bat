@echo off


:: # ------------------------------------------------------------------------------
:: # Environment BT
:: # ------------------------------------------------------------------------------

set ORACLE_SID_TMP=%1
set BT_PLM_SITE=%2

set TC_VERS=tc83
set NX_VERSION=nx85
set BT_ENV=test

set SIDT_APPL=tc_prompt
set SIDT_LANG=en

call %~dp0..\..\nx85_%BT_ENV%_plmshare.cmd

set BT_TC_ROOT_EXTENSION=1PDM4BT

%SystemRoot%\system32\cmd.exe /k %SPLM_SHR_DIR%\start_apps\windows\start_%NX_VERSION%.bat %SIDT_LANG% %SIDT_APPL% %ORACLE_SID% %TC_VERS%



exit
