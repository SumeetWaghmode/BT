::@ECHO OFF
SETLOCAL

SET ARG=%ARG% -h=http://pro1178.server4you.de:33881/tc

SET ARG=%ARG% -u=cadadm
SET ARG=%ARG% -p=cadadm-pdb
SET ARG=%ARG% -g=100_Admin

SET ARG=%ARG% -scr=tc_clone.s#
::SET ARG=%ARG% -mode=BATCH
SET ARG=%ARG% -item="RAS-019501~02"
SET ARG=%ARG% -job_uid="A2I90jkk1cuxKD"


START /B /WAIT %~dp0tc_adapter.cmd %ARG% -mode BATCH
START /B /WAIT %~dp0tc_adapter.cmd %ARG% -mode BATCH_CLONE

ECHO ERRORLEVEL = %ERRORLEVEL%

PAUSE

ENDLOCAL
