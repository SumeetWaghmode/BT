@echo off


:: # ------------------------------------------------------------------------------
:: # Environment BT
:: # ------------------------------------------------------------------------------

set SIDT_APPL=%1
set ORACLE_SID=

set TC_VERS=tc83
set NX_VERSION=nx85
set BT_ENV=prod

set SIDT_LANG=en

call %~dp0..\..\nx85_%BT_ENV%_plmshare.cmd

if "%SIDT_APPL%" == "ALL" goto next

call %SPLM_SHR_DIR%\start_apps\windows\Check_License_Current.cmd
goto end

:next
call %SPLM_SHR_DIR%\start_apps\windows\Check_License_Feature.bat %SIDT_APPL%

:end
exit
