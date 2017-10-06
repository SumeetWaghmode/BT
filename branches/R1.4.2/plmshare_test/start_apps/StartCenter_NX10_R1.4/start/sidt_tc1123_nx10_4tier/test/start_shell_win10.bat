@echo off


:: # ------------------------------------------------------------------------------
:: # Environment BT
:: # ------------------------------------------------------------------------------

set ORACLE_SID_TMP=tcuacst
set BT_PLM_SITE=ZUG

set TC_VERS=tc1123
set NX_VERSION=nx100
set BT_ENV=test

set SIDT_APPL=tc_prompt
set SIDT_LANG=en

call %~dp0..\..\nx100_%BT_ENV%_plmshare.cmd


%SystemRoot%\system32\cmd.exe /k %SPLM_SHR_DIR%\start_apps\windows\start_%NX_VERSION%.bat %SIDT_LANG% %SIDT_APPL% %ORACLE_SID% %TC_VERS%

exit
