::@ECHO OFF
SETLOCAL

SET tit=VarMan Start Script
SET vm_CLArg=--vm_TCItem=RAS-019501/00 --vm_PTKItem=016706/A --vm_TCMode=CLIENT --vm_DataFile="c:\tempugs\temp\vm import.vimp"

IF NOT "%1" == "" SET vm_CLArg=--vm_TCMode=CLIENT %*

SET vm_CLArg=%vm_CLArg: ={SPC}%

SET XLS=%~dp0varman.xlsm
SET EXCEL="%ProgramFiles%\Microsoft Office\OFFICE14\EXCEL.exe"
IF EXIST %EXCEL% GOTO :RUN_APP

SET EXCEL="%ProgramFiles(x86)%\Microsoft Office\OFFICE14\EXCEL.exe"
IF EXIST %EXCEL% GOTO :RUN_APP

SET EXCEL="%ProgramW6432%\Microsoft Office\OFFICE14\EXCEL.exe"
IF EXIST %EXCEL% GOTO :RUN_APP

SET EXCEL="%ProgramFiles%\Microsoft Office\OFFICE12\EXCEL.exe"
IF EXIST %EXCEL% GOTO :RUN_APP

SET EXCEL="%ProgramFiles(x86)%\Microsoft Office\OFFICE12\EXCEL.exe"
IF EXIST %EXCEL% GOTO :RUN_APP

SET EXCEL="%ProgramW6432%\Microsoft Office\OFFICE12\EXCEL.exe"
IF EXIST %EXCEL% GOTO :RUN_APP

SET XLS=%~dp0varman.xls
SET EXCEL="%ProgramFiles%\Microsoft Office\OFFICE11\EXCEL.exe"
IF EXIST %EXCEL% GOTO :RUN_APP

SET EXCEL="%ProgramFiles(x86)%\Microsoft Office\OFFICE11\EXCEL.exe"
IF EXIST %EXCEL% GOTO :RUN_APP

SET EXCEL="%ProgramW6432%\Microsoft Office\OFFICE11\EXCEL.exe"
IF EXIST %EXCEL% GOTO :RUN_APP

SET EXCEL=

:RUN_APP
START /MIN cmd /c START "--" %EXCEL% "%XLS%" %vm_CLArg%

EXIT

ENDLOCAL
