@echo off
rem ******************************************************************************
rem Copyright:  Software Implementation Design Team @ Siemens PLM Software
rem ******************************************************************************
rem
rem  REV    AUTHOR      DATE         COMMENT for splm_nx100.bat
rem  ====   ==========  ==========   =============================================
rem  1.00   A. Patzner  18.02.2015   SIDT Version NX 10.0 1.00
rem  1.01   A. Patzner  17.06.2015   SIDT Version NX 10.0 1.01
rem  1.02   A. Patzner  06.10.2015   SIDT Version NX 10.0 1.02
rem  =============================================================================


echo.
echo --------------------------------
echo start source %~n0
echo %0
echo --------------------------------
echo.

::echo stop at splm_nx100 %TC_VERS% %TC_VERSION%
::pause

rem
rem ------------------------------------------------------------------------------
rem debug variable to set echo on
rem ------------------------------------------------------------------------------
if "%SIDT_DEBUG%" == "1" echo on


rem ------------------------------------------------------------------------------
rem set UG specific variables
rem ------------------------------------------------------------------------------
if not defined UG_VERSION_DIR set UG_VERSION_DIR=nx100
if not defined NX_VERSION_DIR set NX_VERSION_DIR=nx100


rem ------------------------------------------------------------------------------
rem set OS 64bit
rem ------------------------------------------------------------------------------
rem set OS_VERSION=win32
if /i %PROCESSOR_ARCHITECTURE% == AMD64 set OS_VERSION=win64

if exist %SPLM_APPL_DIR%\%NX_VERSION_DIR% goto nx_exists
set NX_INSTALLED=0
goto no_nx_installed

:nx_exists
set NX_INSTALLED=1

if not defined UG_SHR_VERSION_DIR set UG_SHR_VERSION_DIR=nx100
if not defined NX_SHR_VERSION_DIR set NX_SHR_VERSION_DIR=nx100

set UGII_BASE_DIR=%SPLM_APPL_DIR%\%NX_VERSION_DIR%
set UGII_ROOT_DIR=%UGII_BASE_DIR%\ugii\


rem ------------------------------------------------------------------------------
rem set NX temporary directories
rem ------------------------------------------------------------------------------
if not defined UGII_TMP_DIR set UGII_TMP_DIR=%SPLM_TMP_DIR%\nx
set UGII_UGSOLIDS_TMP=%UGII_TMP_DIR%
if not exist %UGII_TMP_DIR% mkdir %UGII_TMP_DIR% > NUL



rem ------------------------------------------------------------------------------
rem Set NX 64bit
rem ------------------------------------------------------------------------------
set NX_PLATFORM=win64
%UGII_BASE_DIR%\ugii\env_print.exe -p | findstr /C:x64wnt > NUL
if %ERRORLEVEL% == 0 set NX_PLATFORM=win64


rem ------------------------------------------------------------------------------
rem variable for ugii_env.dat
rem ------------------------------------------------------------------------------
set UGII_ENV_FILE=%SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM%\ugii\ugii_env.dat


rem ------------------------------------------------------------------------------
rem path for UG font library
rem ------------------------------------------------------------------------------
set UGII_10_FILE=%SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM%\ugii\10.ugf


rem ------------------------------------------------------------------------------
rem language variable for NX
rem ------------------------------------------------------------------------------
if /i "%SIDT_PAR1%" == "de" set UGII_LANG=german
if /i "%SIDT_PAR1%" == "en" set UGII_LANG=english
if /i "%SIDT_PAR1%" == "fr" set UGII_LANG=french
if /i "%SIDT_PAR1%" == "it" set UGII_LANG=italian
if /i "%SIDT_PAR1%" == "es" set UGII_LANG=spanish


rem ------------------------------------------------------------------------------
rem set pattern directory
rem ------------------------------------------------------------------------------
rem set UGII_PATDIR=%SPLM_SHR_DIR%\pattern


rem ------------------------------------------------------------------------------
rem set conditions for filename convention to upper and lowercase
rem ------------------------------------------------------------------------------
set UGII_OPTION=




rem ------------------------------------------------------------------------------
rem set DIN / ISO symbols
rem ------------------------------------------------------------------------------
set UGII_SURFACE_FINISH=ON
set UGII_WELD_SYMBOL=ON
set HOME=%HOMEDRIVE%%HOMEPATH%


rem ------------------------------------------------------------------------------
rem set NX customer directory file
rem ------------------------------------------------------------------------------
set UGII_CUSTOM_DIRECTORY_FILE=%SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM%\ugii\menus\custom_dirs.dat


rem ------------------------------------------------------------------------------
rem thread table files
rem ------------------------------------------------------------------------------
set UGII_ENGLISH_THREADS=%SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM%\ugii\thd_english.dat
set UGII_METRIC_THREADS=%SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM%\ugii\thd_metric.dat


rem ------------------------------------------------------------------------------
rem UDF variables in conjunction mit UG Native
rem ------------------------------------------------------------------------------
set UGII_UDF_DEFINITION_FILE=%SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM%\ugii\udf\dbc_udf_ascii.def
set UGII_UDF_DATABASE_FILE=%SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM%\ugii\udf\udf_database.txt
set UGII_UDF_LIBRARY_DIR=%SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM%\ugii\udf\resource\metric
set UGII_UDF_EVENT_FILE=%SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM%\ugii\udf\dbc_udf_ascii.tcl

rem ------------------------------------------------------------------------------
rem UDF Variables in conjunction mit NX-Manager
rem ------------------------------------------------------------------------------
rem set UGII_UDF_DEFINITION_FILE=%SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM%\ugii\udf\dbc_udf_ugmgr.def
rem set UGII_UDF_EVENT_FILE=%SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM%\ugii\udf\dbc_udf_ugmgr.tcl


rem ------------------------------------------------------------------------------
rem materials Variables
rem ------------------------------------------------------------------------------
set UGII_PHYS_MATERIAL_LIB_DIR=%SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM%\ugii\materials\%UGII_LANG%\
set UGII_PHYS_MATERIAL_LIB_DATA_DIR=%SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM%\ugii\materials\
set UGII_PHYS_MATERIAL_LIB_PATH=.\;%SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM%\ugii\materials\
rem set UGII_USER_MATERIAL_LIB_DIR=%UGII_PHYS_MATERIAL_LIB_PATH%\mvision\%UGII_LANG%\
rem set UGII_USER_MATERIAL_LIB_MODULE=ematl.dll


rem ------------------------------------------------------------------------------
rem set variable for old shared documentation - no PLM Dokumentation Server
rem ------------------------------------------------------------------------------
rem set if /i "%SIDT_PAR1%" == "de" set UGII_UGDOC_BASE_DIR=%SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM%\ugdoc_de
rem set if /i "%SIDT_PAR1%" == "en" set UGII_UGDOC_BASE_DIR=%SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM%\ugdoc_en

rem if exist %SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM%\ugdoc\html_files\nx_help\index.html set UGII_UGDOC_BASE_DIR=%SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM%\ugdoc


rem ------------------------------------------------------------------------------
rem set variable UGII_UGDOC_BASE for shared documentation NX10 
rem fill in the HOSTNAME:PORT from the PLM Documentation Server
rem ------------------------------------------------------------------------------
rem set UGII_UGDOC_BASE=http:\\<SERVERNAME>:8181


rem ------------------------------------------------------------------------------
rem set variable for shared CAST
rem ------------------------------------------------------------------------------
if exist %SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\ugcast\castonly.bat set UGII_CAST=%SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\ugcast\castonly.bat


rem ------------------------------------------------------------------------------
rem set variable for UG HTML Browser
rem ------------------------------------------------------------------------------
if "%OS_LANG%" == "de" set UGII_HTML_BROWSER="%SystemDrive%\Program files\Internet Explorer\IEXPLORE.EXE"
if "%OS_LANG%" == "en" set UGII_HTML_BROWSER="%SystemDrive%\Program files\Internet Explorer\IEXPLORE.EXE"
if "%OS_LANG%" == "fr" set UGII_HTML_BROWSER="%SystemDrive%\Program files\Internet Explorer\IEXPLORE.EXE"
if "%OS_LANG%" == "it" set UGII_HTML_BROWSER="%SystemDrive%\Program files\Internet Explorer\IEXPLORE.EXE"
if "%OS_LANG%" == "es" set UGII_HTML_BROWSER="%SystemDrive%\Program files\Internet Explorer\IEXPLORE.EXE"


rem ------------------------------------------------------------------------------
rem set CAM variables
rem ------------------------------------------------------------------------------
set UGII_CAM_BASE_DIR=%UGII_BASE_DIR%\mach\
set UGII_CAM_CUSTOM_DIR=%SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM%\mach\custom\
set UGII_CAM_RESOURCE_DIR=%SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM%\mach\resource\
set UGII_CAM_FILE_SEARCH_PATH=.\;%UGII_CAM_BASE_DIR%;%SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM%\mach\
set UGII_CAM_LIBRARY_DIR=%UGII_CAM_RESOURCE_DIR%library\
set UGII_CAM_LIBRARY_TOOL_DIR=%UGII_CAM_LIBRARY_DIR%tool\
set UGII_ENABLE_CUTTER_SPIN=1
set UGII_CAM_CM_TM_REFINE=y


rem ------------------------------------------------------------------------------
rem set CMM Inspection Environment Variables
rem ------------------------------------------------------------------------------
set UGII_INSPECTION_BASE_DIR=%UGII_BASE_DIR%\cmm_inspection\
set UGII_INSPECTION_RESOURCE_DIR=%SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM%\cmm_inspection\resource\


rem ------------------------------------------------------------------------------
rem set sheet metal design environment
rem ------------------------------------------------------------------------------
set UGII_SMD_DEFAULTS_FILE=%SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM%\ugii\ug_smd.def
set UGII_UGSMD_STDS_LIBRARY=%SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM%\ugii\


rem ------------------------------------------------------------------------------
rem set nxtools environment
rem ------------------------------------------------------------------------------
if exist %SPLM_SHR_DIR%\nx10_tools\%NX_PLATFORM% goto nxtools_share
if exist %SPLM_APPL_DIR%\nx10_tools goto nxtools_local
goto ende_nxtools_vars

:nxtools_share
set UGTOOLS_SYSDIR=%SPLM_SHR_DIR%\nx10_tools\%NX_PLATFORM%
set NXTOOLS_SYSDIR=%SPLM_SHR_DIR%\nx10_tools\%NX_PLATFORM%
goto nxtools_ok

:nxtools_local
set UGTOOLS_SYSDIR=%SPLM_APPL_DIR%\nx10_tools
set NXTOOLS_SYSDIR=%SPLM_APPL_DIR%\nx10_tools

:nxtools_ok
set UGTOOLS_LOOK_AHEAD=true
set NXTOOLS_LOOK_AHEAD=true
set UGII_USER_TOOLS_FILE=%UGTOOLS_SYSDIR%\usertools\usertools.utd
set UGII_USER_TOOLS_FILE=%NXTOOLS_SYSDIR%\usertools\usertools.utd
set UGII_USER_TOOLS_MENU=%UGTOOLS_SYSDIR%\usertools\usertools.utm
set UGII_USER_TOOLS_MENU=%NXTOOLS_SYSDIR%\usertools\usertools.utm
set UGII_USER_TOOLS_BITMAP_PATH=%UGTOOLS_SYSDIR%\usertools\bitmaps
set UGII_USER_TOOLS_BITMAP_PATH=%NXTOOLS_SYSDIR%\usertools\bitmaps
:ende_nxtools_vars


rem ------------------------------------------------------------------------------
rem set VDAFS environment
rem VDA_LNG sets language to german (D) or english(E)
rem PRT_DIR sets the directory for UG Part files
rem VDA_DIR sets the directory for VDAFS files
rem ------------------------------------------------------------------------------
if exist %SPLM_SHR_DIR%\vdafs_v2.8\%NX_PLATFORM% goto vdafs_share
if exist %SPLM_APPL_DIR%\vdafs_v2.8 goto vdafs_local
goto ende_vdafs_vars

:vdafs_share
set VDAFS_DIR=%SPLM_SHR_DIR%\vdafs_v2.8\%NX_PLATFORM%
goto vdafs_ok

:vdafs_local
set VDAFS_DIR=%SPLM_APPL_DIR%\vdafs_v2.8

:vdafs_ok
rem if /i "%NX_PLATFORM%" == "win32" set UGII_P4_LIBS_DIR=%UGII_BASE_DIR%\ugii
set PATH=%PATH%;%UGII_BASE_DIR%\ugii\
set VDA_LNG=D
set VDA_DIR=%TEMP%
set PRT_DIR=%TEMP%
:ende_vdafs_vars



rem ------------------------------------------------------------------------------
rem set BCT environment
rem ------------------------------------------------------------------------------
if exist %SPLM_APPL_DIR%\bct\16\bct_def\eoms_set.cmd goto bct16.x_exist
goto end_bct_vars

:bct16.x_exist
set BCT_EOMS_DIR=%SPLM_APPL_DIR%\bct\16.1\
call %BCT_EOMS_DIR%\bct_def\eoms_set.cmd %SIDT_PAR1% v28.0
set BCT_3DPOOL_KATALOG=%SPLM_SHR_DIR%\bct\components\
set BCT_COMPONENT_DIR=%BCT_3DPOOL_KATALOG%\variants\
set BCT_SOURCE_INDEX_DIR=%BCT_3DPOOL_KATALOG%\sources\

set BCT_EOMS_TMP=%SPLM_TMP_DIR%\bct
set BCT_PN_WORK_DIR=%BCT_EOMS_TMP%
set EOMS_WORK_DIR=%BCT_EOMS_TMP%\bct_work
rem set BCT_UGII_WORK_DIR=%BCT_EOMS_TMP%
set BCT_WEB_EXCHANGE_DIR=%BCT_EOMS_TMP%
set BCT_INSPECTOR_EXP_DIR=%BCT_EOMS_TMP%
set BCT_EOMS_PLOT_TMP=%BCT_EOMS_TMP%
set HOME=%EOMS_WORK_DIR%

if exist %BCT_EOMS_TMP% goto end_bct_vars
mkdir %BCT_EOMS_TMP% > NUL
mkdir %EOMS_WORK_DIR% > NUL
:end_bct_vars



:no_nx_installed


rem ------------------------------------------------------------------------------
rem set Vis 11.1 environment
rem if more than one version is installed comment out the other versions
rem ------------------------------------------------------------------------------
if exist %SPLM_APPL_DIR%\tcvis1123 goto tcvis11_exists
goto end_tcvis11_vars

:tcvis11_exists
set VIS_VERSION_DIR=%SPLM_APPL_DIR%\tcvis1123
set VIS_VERSION=11
goto end_tcvis11_vars

:end_tcvis11_vars

rem ------------------------------------------------------------------------------
rem set Oracle environment
rem if more than one version is installed comment out the other versions
rem ------------------------------------------------------------------------------
::if exist %SPLM_APPL_DIR%\oracle10g goto oracle10g_exists
if exist %SPLM_APPL_DIR%\oracle11g goto oracle11g_exists
if exist %SPLM_APPL_DIR%\oracle12c goto oracle12c_exists
goto end_oracle_vars

:::oracle10g_exists
::set ORACLE_HOME=%SPLM_APPL_DIR%\oracle10g
::set ORACLE_BIN=%ORACLE_HOME%\bin
::goto end_oracle_vars

:oracle11g_exists
set ORACLE_HOME=%SPLM_APPL_DIR%\oracle11g
set ORACLE_BIN=%ORACLE_HOME%\bin
goto end_oracle_vars

:oracle12c_exists
set ORACLE_HOME=%SPLM_APPL_DIR%\oracle12c
set ORACLE_BIN=%ORACLE_HOME%\bin
goto end_oracle_vars

:end_oracle_vars


rem ----------------------------------------
rem set Teamcenter Engineering environment
rem ----------------------------------------
if "%SIDT_PAR4%" == "TC112" goto set_TC112
if "%SIDT_PAR4%" == "TC1123" goto set_TC1123

goto end_tc_vars


rem -----------------------------------------------------------------------
rem set environment for Tc 91 installation - NOT LONGER SUPPORTED FOR NX10
rem -----------------------------------------------------------------------


:set_TC112
rem -----------------------------------------
rem set environment for Tc 11.2 installation
rem -----------------------------------------
if exist %SPLM_APPL_DIR%\tc11_r14\iiopservers\start_imr.bat                goto 2tier_client
if exist %SPLM_APPL_DIR%\tc\tc11\iiopservers\start_imr.bat                 goto 2tier_client
if exist %SPLM_APPL_DIR%\tc\%TC_VERSION%\midtierservers\midtierservers.properties goto 4tier_client
if exist %SPLM_APPL_DIR%\tc11_r14\portal\client_manifest.mf                goto 4tier_client_otw
REM if exist %SPLM_APPL_DIR%\tc11_r14                                          goto 2tier_client
goto end_tc_vars

:2tier_client
:: C.Franz, no 2tier, jump to 4tier
set TC_ROOT=%SPLM_APPL_DIR%\tc11_r14
set TC_INSTALLED=1
if exist %SPLM_APPL_DIR%\tc\tc11\iiopservers\start_imr.bat set TC_ROOT=%SPLM_APPL_DIR%\tc\tc11
set PORTAL_ROOT_DIR=%TC_ROOT%\portal
set PATH=%PATH%;%TC_ROOT%\bin

goto global_vars

:4tier_client
set TC_ROOT=%SPLM_APPL_DIR%\tc\%TC_VERSION%
set PORTAL_ROOT_DIR=%SPLM_APPL_DIR%\tc\%TC_VERSION%\portal
goto global_vars


:4tier_client_otw
set PORTAL_ROOT_DIR=%SPLM_APPL_DIR%\tc11_r14\portal\rac

:global_vars
set ORACLE_SID=%SIDT_PAR3%
::set TC_VERSION=%SIDT_PAR4%
set PATH=%PATH%;%UGII_BASE_DIR%\ugmanager
rem if exist %SPLM_SHR_DIR%\%ORACLE_SID%data\%OS_VERSION% set TC_DATA=%SPLM_SHR_DIR%\%ORACLE_SID%data\%OS_VERSION%
rem if exist %SPLM_SHR_DIR%\tcdata\%ORACLE_SID%_11data set TC_DATA=%SPLM_SHR_DIR%\tcdata\%ORACLE_SID%_11data
if exist %SPLM_SHR_DIR%\tcdata\%ORACLE_SID%data112 set TC_DATA=%SPLM_SHR_DIR%\tcdata\%ORACLE_SID%data112

set UG_VERSION=V28.0
set PORTAL_LANGUAGE=-Duser.language=%SIDT_PAR1%
if exist %SPLM_SHR_DIR%\pom_transmit set POM_TRANSMIT_DIR=%SPLM_SHR_DIR%\pom_transmit
cd /d %PORTAL_ROOT_DIR%

if not defined TC_TMP_DIR set TC_TMP_DIR=%SPLM_TMP_DIR%\tc
if exist %TC_TMP_DIR% goto plmeasy_vars
mkdir %TC_TMP_DIR% > NUL

:plmeasy_vars
rem Additional environment for PLM Easy
rem -----------------------------------
if exist %SPLM_APPL_DIR%\%ORACLE_SID%local\config\tcpb_addons.bat goto plme_addons_local
if exist %SPLM_SHR_DIR%\%ORACLE_SID%local\config\tcpb_addons.bat goto plme_addons_share
goto end_tc_vars

:plme_addons_local
call %SPLM_APPL_DIR%\%ORACLE_SID%local\config\tcpb_addons.bat
goto end_tc_vars

:plme_addons_share
call %SPLM_SHR_DIR%\%ORACLE_SID%local\config\tcpb_addons.bat
goto end_tc_vars


:set_TC1123
rem -----------------------------------------
rem set environment for Tc 11.2 installation
rem -----------------------------------------
if exist %SPLM_APPL_DIR%\tc\%TC_VERSION%\midtierservers\midtierservers.properties goto 4tier_client
goto end_tc_vars

:4tier_client
set TC_ROOT=%SPLM_APPL_DIR%\tc\%TC_VERSION%
set PORTAL_ROOT_DIR=%SPLM_APPL_DIR%\tc\%TC_VERSION%\portal
goto global_vars

:global_vars
set ORACLE_SID=%SIDT_PAR3%
::set TC_VERSION=%SIDT_PAR4%
set PATH=%PATH%;%UGII_BASE_DIR%\ugmanager

if exist %SPLM_SHR_DIR%\tcdata\%ORACLE_SID%data112 set TC_DATA=%SPLM_SHR_DIR%\tcdata\%ORACLE_SID%data112

set UG_VERSION=V28.0
set PORTAL_LANGUAGE=-Duser.language=%SIDT_PAR1%
if exist %SPLM_SHR_DIR%\pom_transmit set POM_TRANSMIT_DIR=%SPLM_SHR_DIR%\pom_transmit
cd /d %PORTAL_ROOT_DIR%

if not defined TC_TMP_DIR set TC_TMP_DIR=%SPLM_TMP_DIR%\tc
if exist %TC_TMP_DIR% goto plmeasy_vars
mkdir %TC_TMP_DIR% > NUL

:plmeasy_vars
rem Additional environment for PLM Easy
rem -----------------------------------
if exist %SPLM_APPL_DIR%\%ORACLE_SID%local\config\tcpb_addons.bat goto plme_addons_local
if exist %SPLM_SHR_DIR%\%ORACLE_SID%local\config\tcpb_addons.bat goto plme_addons_share
goto end_tc_vars

:plme_addons_local
call %SPLM_APPL_DIR%\%ORACLE_SID%local\config\tcpb_addons.bat
goto end_tc_vars

:plme_addons_share
call %SPLM_SHR_DIR%\%ORACLE_SID%local\config\tcpb_addons.bat
goto end_tc_vars

:end_tc_vars

rem -------------------------------------------------------------------------------------------------------------
rem Customer specific environment must be put in the file %SPLM_SHR_DIR%\start_apps\%OS_SYSTEM%\custom_nx100.bat
rem -------------------------------------------------------------------------------------------------------------
rem
if exist %SPLM_SHR_DIR%\start_apps\%OS_SYSTEM%\custom_nx100.bat call "%SPLM_SHR_DIR%\start_apps\%OS_SYSTEM%\custom_nx100.bat"

echo --------------------------------------
echo End splm_nx100.bat
echo -------------------------------------- 
