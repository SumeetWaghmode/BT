rem @echo on
rem ******************************************************************************
rem Copyright:  Software Implementation Design Team @ Siemens PLM Software
rem ******************************************************************************
rem
rem  REV    AUTHOR      DATE         COMMENT for custom_nx100.bat
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


rem set BT_ENV=prod

rem
rem ------------------------------------------------------------------------------
rem debug variable to set echo on
rem ------------------------------------------------------------------------------
if "%SIDT_DEBUG%" == "1" echo on


:: ############################################################################################################################
:: # Rastatt License Bundle Umstellung, C.Franz 13.10.2014
:: ############################################################################################################################
:: # 1. CAM USER

:: # 151117, CF für das CAM Bundle entfernt user schmierere entfernt
if /i "%USERNAME%" == "gessertm" (
	set UGS_LICENSE_BUNDLE=TOTALMACH
	goto USERSETTING
)

if /i "%USERNAME%" == "malchere" (
	set UGS_LICENSE_BUNDLE=TOTALMACH
	goto USERSETTING
)


rem ------------------------------------------------------------------------------
rem the following entries are only examples
rem ------------------------------------------------------------------------------
rem if /i "%COMPUTERNAME%" == "rcadws39" set UGII_PLOT_NODE=rcadws39
rem if /i "%COMPUTERNAME%" == "rcadws39" set UGII_PRINTER=\\rcadws28\lj4l
rem if /i "%COMPUTERNAME%" == "rcadws39" set UGII_FLEX_BUNDLE=MFG
rem if /i "%USERNAME%" == "maier" set UGII_PRINTER=\\host\share

rem if /i "%COMPUTERNAME%" == "rcadws39" goto set_cam_var
rem goto continue
rem :set_cam_var
rem set UGII_CAM_MOM=1
rem set UGII_CAM_POST_CONFIG_FILE=%SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM%\mach\custom\ugpost\mom_config.dat
rem set UGII_NCD_DIR=%SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM%\mach\custom\tape\
rem set UGII_PPJ_DIR=%SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM%\mach\custom\ugpost\
rem :continue



rem ------------------------------------------------------------------------------
rem
rem Customer specific environment 
rem
rem -----------------------------------------------------------------------------
if "%NX_INSTALLED%" == "0" goto no_nx_installed

:USERSETTING
:: #---------------------------------------------------------------------------------------
:: # Profil für User Einstellungen
:: C.Franz, 21.6.16, Umstellung vom Rastatt: HomeLW p:\ wird nach syncplicity verschoben
:: bei allen Usern, bei denen umgestellt wurde den syncplicity rootpfad auslesen und verwenden
if /i "%COMPUTERNAME_SHORT1%"=="de0w" (
	if exist p:\config (
	set CADUserProfile=P:\config\%NX_SHR_VERSION_DIR%
	set VisUserProfile=P:\config\tcvis83
	) else (
	FOR /F "tokens=3 skip=2" %%i IN ('REG QUERY "HKCU\Software\Syncplicity\1.0" /v RootSyncPointFolder') DO call SET SPLM_ProfileRoot=%%i
	)
)

if /i not "%COMPUTERNAME_SHORT1%"=="de0w" (
	if not "%HOMEDRIVE%"==""  ( 
		set CADUserProfile="%HOMEDRIVE%%HOMEPATH%\config\%NX_SHR_VERSION_DIR%"
		set VisUserProfile="%HOMEDRIVE%%HOMEPATH%\config\tcvis83"
	) else (
		FOR /F "tokens=3 skip=2" %%i IN ('REG QUERY "HKCU\Software\Syncplicity\1.0" /v RootSyncPointFolder') DO call SET SPLM_ProfileRoot=%%i
	)
)	


if not "%SPLM_ProfileRoot%"=="" (
set CADUserProfile=%SPLM_ProfileRoot%\config\%NX_SHR_VERSION_DIR%
set VisUserProfile=%SPLM_ProfileRoot%\config\tcvis83
)


if not exist %CADUserProfile% mkdir %CADUserProfile%
if not exist %VisUserProfile% mkdir %VisUserProfile%


:: #---------------------------------------------------------------------------------------
:: # NX Version ermitteln
if /i "%NX_INSTALLED%" == "0" goto :next
for /f "tokens=*" %%I in (' %UGII_ROOT_DIR%\env_print.exe -n ') do set NX_REV=%%I
:: # NX Patch ermitteln
for /f "tokens=*" %%I in (' %UGII_ROOT_DIR%\env_print.exe -m ') do set NX_PATCH=%%I
if not defined NX_PATCH set NX_PATCH=%NX_REV%

:next
:: #---------------------------------------------------------------------------------------
:: # Pfad fuer Eigene Dateien auslesen
for /f "tokens=3* delims=	" %%i in ('%SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM%\tools\reg query "HKCU\Software\Microsoft\windows\CurrentVersion\Explorer\Shell Folders" /v Personal') do (
   set REG_PATH=%%i
)

:: #---------------------------------------------------------------------------------------
:: # Pfad fuer lokales Applikationsverzeichnis auslesen
for /f "tokens=3* delims=	" %%i in ('%SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM%\tools\reg query "HKCU\Software\Microsoft\windows\CurrentVersion\Explorer\Shell Folders" /v "Local AppData"') do (
   set REG_PATH_Local_AppData=%%i
)

:: #---------------------------------------------------------------------------------------
:: # Pfad fuer lokales Applikationsverzeichnis auslesen
for /f "tokens=3* delims=	" %%i in ('%SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM%\tools\reg query "HKCU\Software\Microsoft\windows\CurrentVersion\Explorer\Shell Folders" /v AppData') do (
   set REG_PATH_AppData=%%i
)

:: #---------------------------------------------------------------------------------------
:: # Pfad fuer lokales Applikationsverzeichnis auslesen
:: cf 18.11.16, this registry key does not exist
::for /f "tokens=3* delims=	" %%i in ('%SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM%\tools\reg query "HKCU\Software\Microsoft\windows\CurrentVersion\Explorer\Shell Folders" /v "Local Settings"') do (
::   set REG_PATH_LocalSettings=%%i
::)




rem ------------------------------------------------------------------------------
rem set UGII_SITE_DIR for site specific customer defaults file
rem the only modification in this file is to set NX to metric and define the ugcolor.cdf file
rem this is SIDT default
rem comment out the variable UGII_LOCAL_USER_DEFAULTS in ugii_env.dat
rem ------------------------------------------------------------------------------
set UGII_SITE_DIR=%SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM%\ugii\site
set UGII_GROUP_DIR=%SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM%\ugii\group
set UGII_LOCAL_USER_DEFAULTS=%CADUserProfile%\nx100_%username%.dpv

set UGII_DRAFTING_STANDARD_DIR=%SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM%\ugii\drafting_standards

set UGII_DEFAULT_ROLE=%SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM%\ugii\site\startup\roles\SBT_base_role.mtx

set UGII_USER_PROFILE_DIR=%CADUserProfile%
if exist "%REG_PATH_Local_AppData%\Unigraphics Solutions\%NX_SHR_VERSION_DIR%\history.pax" (
   move "%REG_PATH_Local_AppData%"\"Unigraphics Solutions"\%NX_SHR_VERSION_DIR%\* %UGII_USER_PROFILE_DIR%\
)

rem ------------------------------------------------------------------------------
rem set SDI plotting config and print_submit directory
rem ------------------------------------------------------------------------------
set UGII_SDI_CONFIG=%SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM%\nxplot_%STANDORT%\config
set UGII_SDI_SERVER_CFG_DIR=%UGII_SDI_CONFIG%\pm_server
rem set UGII_SDI_OVERRIDE_HOME=%UGII_SDI_CONFIG%
rem set UGII_SDI_BPS_JOBS_DIR=%UGII_SDI_CONFIG%

:: # damit Plotter warten und wieder schachteln
set UGII_SDI_HPGL_NO_PS=1

set SDI_WORK=%UGII_TMP_DIR%
set SDI_WORK_DIR=%UGII_TMP_DIR%
set SDI_OVERRIDE_HOME=%UGII_TMP_DIR%


rem ------------------------------------------------------------------------------
rem set variable UGII_UGDOC_BASE for shared documentation NX10 with Doc Server
rem fill in the HOSTNAME:PORT from the current PLM Documentation Server
rem ------------------------------------------------------------------------------
rem set UGII_UGDOC_BASE=http:\\LOCALHOST:8181


:no_nx_installed
rem ------------------------------------------------------------------------------
rem set UGS license server
rem ------------------------------------------------------------------------------
rem set UGS_LICENSE_SERVER=28000@hostname


rem ------------------------------------------------------------------------------
rem set UGS license server (new version)
rem ------------------------------------------------------------------------------
rem set UGS_LICENSE_SERVER=28000@chlupi99002srv.ad001.siemens.net 

set NXN_LICENSE_FILE=%UGS_LICENSE_SERVER%
set SPLM_LICENSE_SERVER=%UGS_LICENSE_SERVER%


rem ------------------------------------------------------------------------------
rem set bundle variable
rem ------------------------------------------------------------------------------
rem set UGII_FLEX_BUNDLE=


rem ------------------------------------------------------------------------------
rem set BCT license server or file
rem ------------------------------------------------------------------------------
rem if exist %SPLM_SHR_DIR%\bct_iecs\bct_license\bctv230.dat set BCT_LICENSE_FILE=%SPLM_SHR_DIR%\bct_iecs\bct_license\bctv220.dat
rem set BCT_LICENSE_FILE=27003@hostname


:: ############################################################################################################################
:: # TEMP environment settings
:: ############################################################################################################################


:: #---------------------------------------------------------------------------------------
:: # Festlegung des TC Konfigurations Verzeichnisses
set DRWE_TC_CONFIG_DIR=%SPLM_SHR_DIR%\tcconfig\%BT_ENV%

:: #---------------------------------------------------------------------------------------
:: # Unigraphics und Tc Temp Umgebung
set SPLM_TMP_DIR=%SystemDrive%\tempugs\temp
set DRWE_TC_HOME=%SystemDrive%\tempugs\%USERNAME%
set DRWE_NUMGEN_TMP=%SystemDrive%\tempugs\numgen

set TEMP=%SPLM_TMP_DIR%
set TMP=%SPLM_TMP_DIR%

set UGII_TMP_DIR=%SPLM_TMP_DIR%\nx
set UGII_UGSOLIDS_TMP=%UGII_TMP_DIR%

set TC_TMP_DIR=%SPLM_TMP_DIR%\tc
set TC_SHARED_MEMORY_DIR=%TC_TMP_DIR%\%USERNAME%
set HOME=%DRWE_TC_HOME%

::Varman
if not exist %SPLM_APPL_DIR%\tc\varman (
   mkdir %SPLM_APPL_DIR%\tc\varman
   echo j|cacls %SPLM_APPL_DIR%\tc\varman /g Jeder:F
   echo y|cacls %SPLM_APPL_DIR%\tc\varman /g Everyone:F
)


:: # synch varman client files to local installation
if exist %DRWE_TC_CONFIG_DIR%\varman\custversion.txt set /p PLM_VARMAN_SERVER_VERSION=<%DRWE_TC_CONFIG_DIR%\varman\custversion.txt
if exist %SPLM_APPL_DIR%\tc\varman\custversion.txt set /p PLM_VARMAN_CLIENT_VERSION=<%SPLM_APPL_DIR%\tc\varman\custversion.txt

if not "X%PLM_VARMAN_SERVER_VERSION%"=="X%PLM_VARMAN_CLIENT_VERSION%" (
	echo updating Varman from version %PLM_VARMAN_CLIENT_VERSION% to version %PLM_VARMAN_SERVER_VERSION%
	robocopy %DRWE_TC_CONFIG_DIR%\varman %SPLM_APPL_DIR%\tc\varman /mir /np /LOG:%TC_TMP_DIR%\..\Varman_%PLM_VARMAN_SERVER_VERSION%.log
)

:: ############################################################################################################################
:: # NX settings
:: ############################################################################################################################

:: #---------------------------------------------------------------------------------------
:: # NX Templates
if /i "%STANDORT%" == "RAS" goto template_ras
set UGII_TEMPLATE_DIR=%SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM%\ugii\templates
goto next

:template_ras
if exist %SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM%\ugii\templates set UGII_TEMPLATE_DIR=%SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM%\ugii\templates_%STANDORT%
:next

:: # ---------------------------------------------------------------------------------------
:: # Color palette (reference in customer defaults file)
set UGII_COLOR=%SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM%\ugii\ugcolor_Metric.cdf

:: # ------------------------------------------------------------------------------
:: # Modeling standards (hole feature)
:: # ------------------------------------------------------------------------------
set UGII_THREADED_HOLE_STANDARD_DIR=%SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM%\ugii\modeling_standards
set UGII_SCREW_CLEARANCE_HOLE_STANDARD_DIR=%SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM%\ugii\modeling_standards

:: #---------------------------------------------------------------------------------------
:: #  The following variable defines the directory that will be used to find
:: #  the Unigraphics Character Font binary files.
set UGII_CHARACTER_FONT_DIR=%SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM%\ugii\ugfonts
set UGII_CHARACTER_FONT_DEFAULT=din

:: #---------------------------------------------------------------------------------------
:: # C.Franz, 8.1.2015, 
:: # IR 8252907  NX Zeichnungserstellung MasterModel Basis-Ansicht)
:: # Siemens' best practice recommendation for creating drawings is to create master model drawings. 
:: # This means the master model resides in one part file while the drawing resides in another part file.   
:: # The drawing file references the data in the master model file.  
:: # Prior to NX 8, when a base view was added to the drawing, the referenced view would default to the model view from the current drawing file.  
:: # This is counter to the master model best practice.  
:: # So a change to the base view dialog was made in NX 8 to default to the views in the master model.  
:: # Users should be aware of this change and understand the referenced views and geometry are now of the master model and not what is in the drawing file. 
:: # If users want the pre-NX 8 behavior, they can change the part option to use the current drawing file and not the master model file or if
:: # they wish to have this as the default for the system they can set the environment variable 
:: # NX_MASTER_MODEL_DWNG_DEFAULT_TO_ROOT_PART=1
set NX_MASTER_MODEL_DWNG_DEFAULT_TO_ROOT_PART=1

:: #---------------------------------------------------------------------------------------
:: # Umgebung fue zusaetzliche Menues setzen
rem if /i "%TC_INSTALLED%" == "0" (

rem if /i "%STANDORT%" == "RAS" goto toolbar_ras
if /i "%SIDT_PAR2%" == "nx" (
   set UGII_ZUSATZMENU_DIR=%SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM%\tools\toolbars\native
rem   copy /Y %SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM_COMMON%\tools\toolbars\native\grip\* c:\temp\ >NUL
) else (
   set UGII_ZUSATZMENU_DIR=%SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM%\tools\toolbars\tc
rem   copy /Y %SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM_COMMON%\tools\toolbars\tc\grip\* c:\temp\ >NUL
)
goto next

:toolbar_ras
if /i "%SIDT_PAR2%" == "nx" (
   set UGII_ZUSATZMENU_DIR=%SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM_COMMON%\tools\toolbars\native_%STANDORT%
   copy /Y %SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM_COMMON%\tools\toolbars\native\grip\* c:\temp\ >NUL
) else (
   set UGII_ZUSATZMENU_DIR=%SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM_COMMON%\tools\toolbars\tc_%STANDORT%
   copy /Y %SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM_COMMON%\tools\toolbars\tc\grip\* c:\temp\ >NUL
)
:next

:: # ---------------------------------------------------------------------------------------
:: # DRWECO Checker & Tools
:: # ---------------------------------------------------------------------------------------
set DRWECO_HOME=%SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM%\tools\drweco
set CDF_FILE_PATH=%SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM%\ugii\ugcolor_Metric.cdf

set USER_FILE=%DRWECO_HOME%\bin\checker.dll
set CHKSTA_ATT_NAME=SF_CHECK_CLASS

:: # ---------------------------------------------------------------------------------------
:: # Accenture Tools
:: # ---------------------------------------------------------------------------------------
set BT_ACCTOOL_BASE_DIR=%SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM%\tools\Accenture

:: # ---------------------------------------------------------------------------------------
:: # SPLM Tools License (NX Drawing Frame)
:: # ---------------------------------------------------------------------------------------
rem set SCH4_TOOL_KEYS_FILE=%SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM%\tools\SSTKey_SiemensBuilding_28000@CHLUPI99002SRV.AD001.SIEMENS.NET.txt

:: # ---------------------------------------------------------------------------------------
:: #  MAYA PCB PRODUCT
:: #
:: #  SBT PCB.xchange configuration
set MAYA_PCB_ENV_DIR=%SPLM_SHR_DIR%\%UG_SHR_VERSION_DIR%\%NX_PLATFORM%\ugpcbxchange

:: # ------------------------------------------------------------------------------
:: # KISSsoft
:: # ------------------------------------------------------------------------------
if exist %SPLM_APPL_DIR%\KISSsoft_03-2016\NX10\64bit set BT_KISS_SOFT_DIR=%SPLM_APPL_DIR%\KISSsoft_03-2016\NX10\64bit
set KSOFT_ICONS=C:\ugs\KISSsoft_03-2016\bin32\icons

:: ############################################################################################################################
:: # CAM Einstellungen
:: ############################################################################################################################

:: #---------------------------------------------------------------------------------------
:: # Cam-Anpassungen
rem if "%COMPUTERNAME%" == "DERASW2098" set UGII_POST_POOL_DIR=%SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM%\mach\resource\postprocessor\
rem if "%COMPUTERNAME%" == "DERASW2676" set UGII_POST_POOL_DIR=%SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM%\mach\resource\postprocessor\
rem if "%COMPUTERNAME%" == "DE0W30089"  set UGII_POST_POOL_DIR=%SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM%\mach\resource\postprocessor\

rem set UGII_CAM_BASE_DIR=%UGII_BASE_DIR%\mach\
rem set UGII_CAM_CUSTOM_DIR=%SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM_COMMON%\mach_%STANDORT%\custom\
rem 24.03.2014 M.Rapp Janus: Vereinheitlicht, über untersch. Konfigurationen werden die untersch. Konfigurationen gesteuert
set UGII_CAM_CUSTOM_DIR=%SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM%\mach\custom\
rem set UGII_CAM_RESOURCE_DIR=%SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM_COMMON%\mach\resource\
set UGII_CAM_FILE_SEARCH_PATH=.\;%UGII_CAM_BASE_DIR%;%SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM%\mach\;%SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM%\mach_%STANDORT%\
rem 08.07.2014 M.Andersohn Janus: UGII_CAM_POST_CONFIG_FILE mit Berücksichtigung Standort Variable
set UGII_CAM_POST_CONFIG_FILE=%UGII_CAM_CUSTOM_DIR%postprocessor\template_post_%STANDORT%.dat
set UGII_CAM_LIBRARY_DIR=%UGII_CAM_CUSTOM_DIR%library\
set UGII_CAM_LIBRARY_TOOL_DIR=%UGII_CAM_LIBRARY_DIR%tool\
rem set UGII_ENABLE_CUTTER_SPIN=1
rem set UGII_CAM_CM_TM_REFINE=y

rem rem set UGII_CAM_CONFIG_DIR=%UGII_CAM_RESOURCE_DIR%configuration\
set UGII_CAM_CONFIG_DIR=%UGII_CAM_CUSTOM_DIR%configuration\
if /i "%STANDORT%" == "ZUG" set UGII_CAM_CONFIG=%UGII_CAM_CONFIG_DIR%siemens_general_zug.dat
if /i "%STANDORT%" == "RAS" set UGII_CAM_CONFIG=%UGII_CAM_CONFIG_DIR%siemens_general_ras.dat


rem if /i "%STANDORT%" == "RAS" set UGII_POST_POOL_DIR=%SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM_COMMON%\mach_%STANDORT%\custom\postprocessor\
rem 24.03.2014 M.Rapp Janus: UGII_CAM_POST_DIR ist nicht mehr standortspez. dies wird über die konfiguration gesteuert
set UGII_CAM_POST_DIR=%SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM%\mach\custom\postprocessor\

set UGII_CAM_TEMPLATE_PART_DIR=%UGII_CAM_CUSTOM_DIR%template_part\
set UGII_CAM_LIBRARY_TEPMLATE_DIR=%UGII_CAM_CUSTOM_DIR%template_dir\
set UGII_CAM_LIBRARY_TEPMLATE_DATA_DIR=%UGII_CAM_CUSTOM_DIR%template_dir\
set UGII_CAM_TEMPLATE_SET_DIR=%UGII_CAM_CUSTOM_DIR%template_set\

rem rem Zug hat UGII_CAM_RESOURCE_DIR im ...\nx85\win\mach_zug und nicht separat unter ...\nx85\win64\mach
rem if /i "%STANDORT%" == "ZUG" set UGII_CAM_RESOURCE_DIR=%UGII_CAM_CUSTOM_DIR%
rem 24.03.2014 M.Rapp Janus: standortunabhängig, da beide Standorte dieselben Daten verwenden wollen
set UGII_CAM_RESOURCE_DIR=%UGII_CAM_CUSTOM_DIR%

:: #---------------------------------------------------------------------------------------
:: # Janus Tools
rem set JS_TOOLS_DIR=%SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM%\tools\janus_tools

:: ############################################################################################################################
:: # NX Manager settings
:: ############################################################################################################################

:: # ---------------------------------------------------------------------------------------
:: # Variant configuration menu under Assembly main menu
rem set TC_USE_TEST_ATTR_MAPPINGS=1
set UGII_DEBUG=UGMGR_attr_mapping
set UGII_UGMGR_OLD_VARIANTS=1

:: # ---------------------------------------------------------------------------------------
:: # IMM - Integrated Material Management
set SPECIAL_TOOLS_DIR=%SPLM_SHR_DIR%\%NX_SHR_VERSION_DIR%\%NX_PLATFORM%\tools\imm


:: ############################################################################################################################
:: # Tc Einstellungen
:: ############################################################################################################################

:: # synch plugin configuration  client files to local installation
rem cf, 8.11.16 umstellung auf die switch scripte von p.winkler
rem if exist %DRWE_TC_CONFIG_DIR%\portal\custversion.txt set /p PLUGIN_SERVER_VERSION=<%DRWE_TC_CONFIG_DIR%\portal\custversion.txt
rem if exist %SPLM_APPL_DIR%\tc\112_plugins\%BT_ENV%\custversion.txt set /p PLUGIN_CLIENT_VERSION=<%SPLM_APPL_DIR%\tc\112_plugins\%BT_ENV%custversion.txt
rem 
rem if not "X%PLUGIN_SERVER_VERSION%"=="X%PLUGIN_CLIENT_VERSION%" (
rem 	echo updating client plugins from version %PLUGIN_CLIENT_VERSION% to version %PLUGIN_SERVER_VERSION%
rem 	set CPYT_VERSION=%TC_ROOT%\temp
rem 	copy /Y /V %SPLM_SHR_DIR%\tcconfig\%BT_ENV%\portal\custversion.txt %CPYT_VERSION%
rem )
rem :: Set the Copy Targets
rem set CPYT_INST=%TC_ROOT%\install
rem set CPYT_MIDT=%TC_ROOT%\midtierservers
rem set CPYT_CONF=%TC_ROOT%\portal\plugins\configuration_11000.2.0
rem set CPYT_FMS=%TC_ROOT%\tccs
rem 
rem 
rem copy /Y /V %SPLM_SHR_DIR%\tcconfig\%BT_ENV%\portal\configuration\midtierservers.properties %CPYT_MIDT%
rem copy /Y /V %SPLM_SHR_DIR%\tcconfig\%BT_ENV%\portal\configuration\configuration.xml %CPYT_INST%
rem copy /Y /V %SPLM_SHR_DIR%\tcconfig\%BT_ENV%\portal\configuration\client_specific.properties %CPYT_CONF%
rem copy /Y /V %SPLM_SHR_DIR%\tcconfig\%BT_ENV%\portal\configuration\fcc.xml %CPYT_FMS%
rem 
rem 
rem call %SPLM_SHR_DIR%\tcconfig\%BT_ENV%\portal\genregxml.bat

:: cf, 18.11.16 patch and switch only for clients. manual configuration on server

:: switch darf nur auf dem Client ausgeführt werden. Damit die Server nicht gepatcht werden muß im Server
:: environment BT_SERVER=prod_sever gesetzt sein
if /i "%BT_SERVER%" NEQ "prod_server" (
:: cf, tc client configuration, switch script p.winkler
call %SPLM_SHR_DIR%\clientcfg\switch_4tier_clientcfg.bat %BT_PLM_SITE% %BT_ENV% 

:: cf, client patching, skript p.winkler
call %SPLM_SHR_DIR%\tcconfig\%BT_ENV%\patch\patch_4tier_client.bat %BT_ENV%

::echo stop at %~n0 %TC_VERSION% %BT_ENV%
::pause

)
:: Powerbox Settings
set TCPB_LOCAL=%SPLM_SHR_DIR%\tcpb112local
::set TCPB_LOCAL=%SPLM_SHR_DIR%\tcpb%TC_VERSION%local

:: #---------------------------------------------------------------------------------------
:: # DRWECO Variantenmanagement
set DRWECO_VM_DIR=%SPLM_APPL_DIR%\tc\VarMan
set DRWECO_VM_START_SCRIPT=%DRWECO_VM_DIR%\varman.cmd
rem set DRWECO_VM_WEB_URL=%BT_TC_WEB_PROTOKOLL%://%BT_TC_WEB_SERVER%:%BT_TC_WEB_PORT%/tc
rem set DRWECO_VM_WEB_URL=http://139.16.88.142:7005/tc
rem set DRWECO_VM_WEB_URL=http://185.48.117.113:7001/tc

rem set the environment variable dendent on the instance
rem 1) Inistlaize with central site
set DRWECO_VM_WEB_URL=http://chlupi99007srv.ad001.siemens.net:51000/tc
rem 2) Now change in case the instance differs
if /i "%BT_ENV%" == "int" set DRWECO_VM_WEB_URL=http://ch1w80166.ad001.siemens.net:7001/tc
if /i "%BT_ENV%" == "test" set DRWECO_VM_WEB_URL=http://chlupi99014srv.ad001.siemens.net:51000/tc

rem stop running fcc
rem stop running fcc
set FMS_HOME=%TC_ROOT%\tccs
%TC_ROOT%\tccs\bin\fccstat.exe -kill


rem 
rem :: #---------------------------------------------------------------------------------------
rem :: # set config dir variables
rem set SBT_CONFIG_DIR=%SPLM_SHR_DIR%\tcconfig
rem 
rem :: #---------------------------------------------------------------------------------------
rem :: # TCCS Variablen setzen
rem set FMS_HOME=%TC_ROOT%\tccs
rem set SBT_FCC_CONFIG=%SBT_CONFIG_DIR%\prod\fcc\fcc_%ORACLE_SID%.xml
rem 
rem set FCC_PROXYPIPENAME=\\.\pipe\%ORACLE_SID%_FMSClientPipe
rem 

echo ________________________________________________________________________________
echo.
echo    NX100 Enviroment on: %BT_UG_SERVER%
echo ________________________________________________________________________________
echo.
echo    UserName:            %USERNAME%
echo    Computername:        %COMPUTERNAME%
echo    Site:                %STANDORT%
echo    Group:               %DRWE_UGII_GROUP%
echo    My Documents:        %REG_PATH%
echo    AppData:             %REG_PATH_AppData%
echo    Local AppData:       %REG_PATH_Local_AppData%
if /i "%TC_INSTALLED%" == "1" echo.
if /i "%TC_INSTALLED%" == "1" echo    TC_ROOT:             %TC_ROOT%
if /i "%TC_INSTALLED%" == "1" echo    TC_DATA:             %TC_DATA%
if /i "%TC_INSTALLED%" == "1" echo    TCPB_LOCAL:          %TCPB_LOCAL%
if /i "%TC_INSTALLED%" == "1" echo.
if /i "%TC_INSTALLED%" == "1" echo    ORACLE:              %ORACLE_SID%
if /i "%TC_INSTALLED%" == "1" echo.
echo    SPLM_SHR_DIR:        %SPLM_SHR_DIR%
echo    UGS_LICENSE_SERVER:  %UGS_LICENSE_SERVER%
echo    UGS_LICENSE_BUNDLE:  %UGS_LICENSE_BUNDLE%
echo.
if /i "%NX_INSTALLED%" == "1" echo    UGII_BASE_DIR:       %UGII_BASE_DIR% - %UGII_LANG% - %OS_VERSION%
if /i "%NX_INSTALLED%" == "1" echo.
if /i "%NX_INSTALLED%" == "1" echo    NX Patch Version:    %NX_PATCH%
if /i "%TC_INSTALLED%" == "1" echo    TC Patch Version:    TC %TcVersion%
echo.
if /i "%TC_INSTALLED%" == "1" echo    Last TC Reg Version: %DRWE_TC_REG_TXT1%
echo.
echo ________________________________________________________________________________
if "%SIDT_DEBUG%" == "1" @echo on


echo --------------------------------------
echo End custom_nx100.bat
echo -------------------------------------- 

