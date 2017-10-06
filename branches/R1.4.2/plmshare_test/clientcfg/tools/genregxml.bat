@echo off
setlocal

call "C:\ugs\tc\%TC_VERSION%\install\tem_init.bat"

set TPR=C:\ugs\tc\%TC_VERSION%\portal
set JRE_HOME=%TC_JRE_HOME%
set CMD=call Teamcenter.exe -vm "%JRE_HOME%\bin\javaw.exe" -application com.teamcenter.rac.util.RegistryLoader -nosplash
set VMARGS=-vmargs -Xmx128m

cd /d "%TPR%"

::%CMD% -nl cs_CZ %VMARGS%
::%CMD% -nl de %VMARGS%
%CMD% -nl en %VMARGS%
::%CMD% -nl es %VMARGS%
::%CMD% -nl fr %VMARGS%
::%CMD% -nl it %VMARGS%
::%CMD% -nl ja_JP %VMARGS%
::%CMD% -nl ko_KR %VMARGS%
::%CMD% -nl pl_PL %VMARGS%
::%CMD% -nl pt_BR %VMARGS%
::%CMD% -nl ru_RU %VMARGS%
::%CMD% -nl zh_CN %VMARGS%
::%CMD% -nl zh_TW %VMARGS%

:END_SCRIPT
exit /b %ERRORLEVEL%
endlocal
echo on
