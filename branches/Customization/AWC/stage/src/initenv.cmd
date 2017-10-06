@REM @<COPYRIGHT>@
@REM ============================================================================
@REM Copyright 2015.
@REM Siemens Product Lifecycle Management Software Inc.
@REM All Rights Reserved.
@REM ============================================================================
@REM @<COPYRIGHT>@

@REM Check to see IF we are an IIS deployment, IF so set flag
set IIS_DEPLOYMENT=true
IF '%1' == 'iis' @ECHO +++ This is an IIS Deployment
IF '%1' == 'iis' set IIS_DEPLOYMENT=true

@REM Java settings - Note, this odd looking statement is needed for support TEM's setting of AWC Installer path.
set AWS2_STAGE_DIR=%~dps0

@REM the Active Workspace Client stage directory
set AWC_STAGE_DIR=%AWS2_STAGE_DIR%
IF NOT DEFINED AWC_STAGE_DIR @ECHO AWC_STAGE_DIR NOT DEFINED & EXIT /B 1

set ROOT=%AWC_STAGE_DIR%
set JS_SCRIPTS=%AWC_STAGE_DIR%\build

@REM bin directory to PATH
SET PATH=%AWC_STAGE_DIR%\bin;%PATH%
