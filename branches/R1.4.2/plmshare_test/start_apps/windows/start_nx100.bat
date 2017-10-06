@echo off
rem ******************************************************************************
rem Copyright:  Software Implementation Design Team @ Siemens PLM Software
rem ******************************************************************************
rem
rem  REV    AUTHOR      DATE         COMMENT for start_nx100.bat
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


rem ------------------------------------------------------------------------------
rem save start parameters
rem ------------------------------------------------------------------------------
set SIDT_PAR1=%1
set SIDT_PAR2=%2
set SIDT_PAR3=%3
set SIDT_PAR4=%4


rem ------------------------------------------------------------------------------
rem set OLD SIDT-Variables - will be obsolete in SIDT V9.0
rem ------------------------------------------------------------------------------
if not defined SPLM_APPL_DIR set SPLM_APPL_DIR=C:\ugs
set UGS_APPL_DIR=%SPLM_APPL_DIR%
rem set UGS_TMP_DIR=%SPLM_TMP_DIR%
set SPLM_TMP_DIR=C:\tempugs\temp


rem ------------------------------------------------------------------------------
rem set initial variables
rem ------------------------------------------------------------------------------
if not defined OS_SYSTEM set OS_SYSTEM=windows
set HIDE_WINDOW=1


rem ------------------------------------------------------------------------------
rem debug variable to switch echo on
rem ------------------------------------------------------------------------------
if "%SIDT_DEBUG%" == "1" goto debug_options
goto no_debug

:debug_options
set HIDE_WINDOW=0
@echo on
:no_debug

rem ------------------------------------------------------------------------------
rem check parameter %1
rem ------------------------------------------------------------------------------
rem
if "%SIDT_PAR1%" == "-v" goto check_version
if ("%SIDT_PAR1%" == "/?") or ("%SIDT_PAR1%" == "") goto start_help
if /i "%SIDT_PAR1%" == "de" call "%SPLM_SHR_DIR%\start_apps\%OS_SYSTEM%\splm_nx100.bat"
if /i "%SIDT_PAR1%" == "en" call "%SPLM_SHR_DIR%\start_apps\%OS_SYSTEM%\splm_nx100.bat"
if /i "%SIDT_PAR1%" == "fr" call "%SPLM_SHR_DIR%\start_apps\%OS_SYSTEM%\splm_nx100.bat"
if /i "%SIDT_PAR1%" == "it" call "%SPLM_SHR_DIR%\start_apps\%OS_SYSTEM%\splm_nx100.bat"
if /i "%SIDT_PAR1%" == "es" call "%SPLM_SHR_DIR%\start_apps\%OS_SYSTEM%\splm_nx100.bat"


rem -------------------------------------------------------------------------------------
rem starting the application, depending on parameter %SIDT_PAR2%
rem -------------------------------------------------------------------------------------
rem
goto start_%SIDT_PAR2%

:start_nxpartfile
if %HIDE_WINDOW% == 1 call %SPLM_SHR_DIR%\start_apps\%OS_SYSTEM%\tools\cmdow.exe @ /hid 
%UGII_BASE_DIR%\UGII\ugs_router.exe -ug -use_file_dir %3
goto end

:start_nx
if %HIDE_WINDOW% == 1 call %SPLM_SHR_DIR%\start_apps\%OS_SYSTEM%\tools\cmdow.exe @ /hid 
%UGII_BASE_DIR%\UGII\ugraf.exe -nx
goto end

:start_nx_powerdraft
if %HIDE_WINDOW% == 1 call %SPLM_SHR_DIR%\start_apps\%OS_SYSTEM%\tools\cmdow.exe @ /hid 
%UGII_BASE_DIR%\UGII\ugraf.exe -powerdrafting
goto end

:start_nx_mechatronics
if %HIDE_WINDOW% == 1 call %SPLM_SHR_DIR%\start_apps\%OS_SYSTEM%\tools\cmdow.exe @ /hid 
%UGII_BASE_DIR%\UGII\ugraf.exe -mechatronics
goto end

:start_nx_viewer
if %HIDE_WINDOW% == 1 call %SPLM_SHR_DIR%\start_apps\%OS_SYSTEM%\tools\cmdow.exe @ /hid 
%UGII_BASE_DIR%\UGII\ugraf.exe -view
goto end

:start_nx_prompt
%systemroot%\system32\cmd.exe /k %UGII_BASE_DIR%\UGII\ugiicmd.bat %UGII_BASE_DIR%\
goto end

:start_kf_ice
%UGII_BASE_DIR%\UGII\ice.bat %UGII_BASE_DIR%\
goto end

:start_nxopen_grip
%UGII_BASE_DIR%\ugopen\grade.bat %UGII_BASE_DIR%\
goto end

:start_plotadmin
set PATH=%UGII_ROOT_DIR%;%PATH%
if %HIDE_WINDOW% == 1 call %SPLM_SHR_DIR%\start_apps\%OS_SYSTEM%\tools\cmdow.exe @ /hid 
%UGII_BASE_DIR%\nxplot\nxplotadmin.exe
goto end

:start_plotmenue
set PATH=%UGII_ROOT_DIR%;%PATH%
if %HIDE_WINDOW% == 1 call %SPLM_SHR_DIR%\start_apps\%OS_SYSTEM%\tools\cmdow.exe @ /hid 
%UGII_BASE_DIR%\nxplot\nxplot.exe
goto end

:start_vre
if %HIDE_WINDOW% == 1 call %SPLM_SHR_DIR%\start_apps\%OS_SYSTEM%\tools\cmdow.exe @ /hid 
%UGII_BASE_DIR%\DESIGN_TOOLS\checkmate\tools\ruleset_editor\ruleeditor.bat
goto end

:start_pts
if %HIDE_WINDOW% == 1 call %SPLM_SHR_DIR%\start_apps\%OS_SYSTEM%\tools\cmdow.exe @ /hid 
%UGII_BASE_DIR%\UGII\pts.bat %UGII_BASE_DIR%\
goto end

:start_qdb
if %HIDE_WINDOW% == 1 call %SPLM_SHR_DIR%\start_apps\%OS_SYSTEM%\tools\cmdow.exe @ /hid 
%UGII_BASE_DIR%\DESIGN_TOOLS\checkmate\tools\quality_dashboard\qdashboard.bat
goto end

:start_nxflexlm
if %HIDE_WINDOW% == 1 call %SPLM_SHR_DIR%\start_apps\%OS_SYSTEM%\tools\cmdow.exe @ /hid 
%UGII_BASE_DIR%\UGII\licensing_tool.exe
goto end

:start_flex_server
if %HIDE_WINDOW% == 1 call %SPLM_SHR_DIR%\start_apps\%OS_SYSTEM%\tools\cmdow.exe @ /hid 
%SPLM_APPL_DIR%\UGFLEXLM\lmtools.exe
goto end

:start_flex_client
if %HIDE_WINDOW% == 1 call %SPLM_SHR_DIR%\start_apps\%OS_SYSTEM%\tools\cmdow.exe @ /hid 
%UGII_BASE_DIR%\UGII\licensing_tool.exe
goto end

:start_ascmdf
%UGII_BASE_DIR%\MACH\auxiliary\ascmdf.bat %UGII_BASE_DIR%\
goto end

:start_cls2clf
%UGII_BASE_DIR%\MACH\auxiliary\cls2clf.bat %UGII_BASE_DIR%\
goto end

:start_gpm
if %HIDE_WINDOW% == 1 call %SPLM_SHR_DIR%\start_apps\%OS_SYSTEM%\tools\cmdow.exe @ /hid 
%UGII_BASE_DIR%\MACH\auxiliary\rungpm.bat %UGII_BASE_DIR%\
goto end

:start_mdfg
%UGII_BASE_DIR%\MACH\auxiliary\mdfg.bat %UGII_BASE_DIR%\
goto end

:start_postbuild
if %HIDE_WINDOW% == 1 call %SPLM_SHR_DIR%\start_apps\%OS_SYSTEM%\tools\cmdow.exe @ /hid 
%UGII_BASE_DIR%\POSTBUILD\post_builder.bat %UGII_BASE_DIR%\
goto end

:start_nxpost
if %HIDE_WINDOW% == 1 call %SPLM_SHR_DIR%\start_apps\%OS_SYSTEM%\tools\cmdow.exe @ /hid 
%UGII_BASE_DIR%\MACH\auxiliary\runugpost.bat %UGII_BASE_DIR%\
goto end

:start_xlator
%UGII_BASE_DIR%\MACH\auxiliary\xlator.bat" %UGII_BASE_DIR%\
goto end

:start_mkeditor
%UGII_BASE_DIR%\MACH\machining_knowledge_editor\bin\MkEditor.exe
goto end

:start_camexpress
%UGII_BASE_DIR%\UGII\ugraf.exe -camexpress
goto end

:start_camsme
%UGII_BASE_DIR%\UGII\ugraf.exe -camsme
goto end

:start_catiav4
if %HIDE_WINDOW% == 1 call %SPLM_SHR_DIR%\start_apps\%OS_SYSTEM%\tools\cmdow.exe @ /hid 
%UGII_BASE_DIR%\UGCATIA\catiav4.bat %UGII_BASE_DIR%\CATIAV4 %UGII_BASE_DIR% AUTO
goto end

:start_catiav5
if %HIDE_WINDOW% == 1 call %SPLM_SHR_DIR%\start_apps\%OS_SYSTEM%\tools\cmdow.exe @ /hid 
%UGII_BASE_DIR%\CATIAV5\catiav5.bat %UGII_BASE_DIR%\CATIAV5 %UGII_BASE_DIR% AUTO
goto end

:start_dxfdwg
if %HIDE_WINDOW% == 1 call %SPLM_SHR_DIR%\start_apps\%OS_SYSTEM%\tools\cmdow.exe @ /hid 
%UGII_BASE_DIR%\DXFDWG\dxfdwg.bat %UGII_BASE_DIR%\DXFDWG %UGII_BASE_DIR% auto
goto end

:start_iges
if %HIDE_WINDOW% == 1 call %SPLM_SHR_DIR%\start_apps\%OS_SYSTEM%\tools\cmdow.exe @ /hid 
%UGII_BASE_DIR%\IGES\iges.bat %UGII_BASE_DIR%\IGES %UGII_BASE_DIR% auto
goto end

:start_nxproe
if %HIDE_WINDOW% == 1 call %SPLM_SHR_DIR%\start_apps\%OS_SYSTEM%\tools\cmdow.exe @ /hid 
%UGII_BASE_DIR%\NXPROE\nxproe.bat %UGII_BASE_DIR%\NXPROE %UGII_BASE_DIR% auto
goto end

:start_ugto2d
%UGII_BASE_DIR%\UGTO2D\ugto2d.bat %UGII_BASE_DIR%\UGTO2D %UGII_BASE_DIR% auto
goto end

:start_step203ug
if %HIDE_WINDOW% == 1 call %SPLM_SHR_DIR%\start_apps\%OS_SYSTEM%\tools\cmdow.exe @ /hid 
%UGII_BASE_DIR%\STEP203UG\step203ug.bat %UGII_BASE_DIR%\STEP203UG %UGII_BASE_DIR% auto
goto end

:start_step214ug
if %HIDE_WINDOW% == 1 call %SPLM_SHR_DIR%\start_apps\%OS_SYSTEM%\tools\cmdow.exe @ /hid 
%UGII_BASE_DIR%\STEP214UG\step214ug.bat %UGII_BASE_DIR%\STEP214UG %UGII_BASE_DIR% auto
goto end

:start_vda2ug
if %HIDE_WINDOW% == 1 call %SPLM_SHR_DIR%\start_apps\%OS_SYSTEM%\tools\cmdow.exe @ /hid 
"%VDAFS_DIR%\vda_to_ug_ui.exe" "%VDA_LNG%" "%VDA_DIR%" "%PRT_DIR%"
goto end

:start_ug2vda
if %HIDE_WINDOW% == 1 call %SPLM_SHR_DIR%\start_apps\%OS_SYSTEM%\tools\cmdow.exe @ /hid 
"%VDAFS_DIR%\ug_to_vda_ui.exe" "%VDA_LNG%" "%PRT_DIR%" "%VDA_DIR%"
goto end


rem -----------------------------------------------------------------------------
rem Start Teamcenter Visualization 9.1 processes - NOT LONGER SUPPORTED FOR NX10
rem -----------------------------------------------------------------------------


rem -------------------------------------------------------------------------
rem Start Teamcenter Visualization 10.1 processes
rem -------------------------------------------------------------------------
:start_tcvis10_lic_util
if %HIDE_WINDOW% == 1 call %SPLM_SHR_DIR%\start_apps\%OS_SYSTEM%\tools\cmdow.exe @ /hid 
%VIS_VERSION_DIR%\program\VisLicenseBorrow.exe
goto end

:start_tcvis10_iseries_base
if %HIDE_WINDOW% == 1 call %SPLM_SHR_DIR%\start_apps\%OS_SYSTEM%\tools\cmdow.exe @ /hid 
%VIS_VERSION_DIR%\products\base\iSeriesLauncher.exe Base
goto end

:start_tcvis10_iseries_pro
if %HIDE_WINDOW% == 1 call %SPLM_SHR_DIR%\start_apps\%OS_SYSTEM%\tools\cmdow.exe @ /hid 
%VIS_VERSION_DIR%\products\professional\iSeriesLauncher.exe Pro
goto end

:start_tcvis10_iseries_std
if %HIDE_WINDOW% == 1 call %SPLM_SHR_DIR%\start_apps\%OS_SYSTEM%\tools\cmdow.exe @ /hid 
%VIS_VERSION_DIR%\products\standard\iSeriesLauncher.exe Std
goto end

:start_tcvis10_base
if %HIDE_WINDOW% == 1 call %SPLM_SHR_DIR%\start_apps\%OS_SYSTEM%\tools\cmdow.exe @ /hid 
%VIS_VERSION_DIR%\products\Base\VisView.exe
goto end

:start_tcvis10_std
if %HIDE_WINDOW% == 1 call %SPLM_SHR_DIR%\start_apps\%OS_SYSTEM%\tools\cmdow.exe @ /hid 
%VIS_VERSION_DIR%\products\Standard\VisView.exe
goto end

:start_tcvis10_pro
if %HIDE_WINDOW% == 1 call %SPLM_SHR_DIR%\start_apps\%OS_SYSTEM%\tools\cmdow.exe @ /hid 
%VIS_VERSION_DIR%\products\Professional\VisView.exe
goto end

:start_tcvis10_mockup
if %HIDE_WINDOW% == 1 call %SPLM_SHR_DIR%\start_apps\%OS_SYSTEM%\tools\cmdow.exe @ /hid 
%VIS_VERSION_DIR%\products\MockUp\VisView.exe
goto end

:start_tcvis10_print
if %HIDE_WINDOW% == 1 call %SPLM_SHR_DIR%\start_apps\%OS_SYSTEM%\tools\cmdow.exe @ /hid 
%VIS_VERSION_DIR%\VVCP\pvprint.exe
goto end

:start_tcvis10_convert
if %HIDE_WINDOW% == 1 call %SPLM_SHR_DIR%\start_apps\%OS_SYSTEM%\tools\cmdow.exe @ /hid
%VIS_VERSION_DIR%\VVCP\prepare.exe
goto end

rem -------------------------------------------------------------------------
rem Start Teamcenter Visualization 11.1 processes
rem -------------------------------------------------------------------------
:start_tcvis11_lic_util
if %HIDE_WINDOW% == 1 call %SPLM_SHR_DIR%\start_apps\%OS_SYSTEM%\tools\cmdow.exe @ /hid 
%VIS_VERSION_DIR%\program\VisLicenseBorrow.exe
goto end

:start_tcvis11_iseries_base
if %HIDE_WINDOW% == 1 call %SPLM_SHR_DIR%\start_apps\%OS_SYSTEM%\tools\cmdow.exe @ /hid 
%VIS_VERSION_DIR%\products\base\iSeriesLauncher.exe Base
goto end

:start_tcvis11_iseries_pro
if %HIDE_WINDOW% == 1 call %SPLM_SHR_DIR%\start_apps\%OS_SYSTEM%\tools\cmdow.exe @ /hid 
%VIS_VERSION_DIR%\products\professional\iSeriesLauncher.exe Pro
goto end

:start_tcvis11_iseries_std
if %HIDE_WINDOW% == 1 call %SPLM_SHR_DIR%\start_apps\%OS_SYSTEM%\tools\cmdow.exe @ /hid 
%VIS_VERSION_DIR%\products\standard\iSeriesLauncher.exe Std
goto end

:start_tcvis11_base
if %HIDE_WINDOW% == 1 call %SPLM_SHR_DIR%\start_apps\%OS_SYSTEM%\tools\cmdow.exe @ /hid 
%VIS_VERSION_DIR%\products\Base\VisView.exe
goto end

:start_tcvis11_std
if %HIDE_WINDOW% == 1 call %SPLM_SHR_DIR%\start_apps\%OS_SYSTEM%\tools\cmdow.exe @ /hid 
%VIS_VERSION_DIR%\products\Standard\VisView.exe
goto end

:start_tcvis11_pro
if %HIDE_WINDOW% == 1 call %SPLM_SHR_DIR%\start_apps\%OS_SYSTEM%\tools\cmdow.exe @ /hid 
%VIS_VERSION_DIR%\products\Professional\VisView.exe
goto end

:start_tcvis11_mockup
if %HIDE_WINDOW% == 1 call %SPLM_SHR_DIR%\start_apps\%OS_SYSTEM%\tools\cmdow.exe @ /hid 
%VIS_VERSION_DIR%\products\MockUp\VisView.exe
goto end

:start_tcvis11_print
if %HIDE_WINDOW% == 1 call %SPLM_SHR_DIR%\start_apps\%OS_SYSTEM%\tools\cmdow.exe @ /hid 
%VIS_VERSION_DIR%\VVCP\pvprint.exe
goto end

:start_tcvis11_convert
if %HIDE_WINDOW% == 1 call %SPLM_SHR_DIR%\start_apps\%OS_SYSTEM%\tools\cmdow.exe @ /hid
%VIS_VERSION_DIR%\VVCP\prepare.exe
goto end

rem -------------------------------------------------------------------------
rem Start Teamcenter processes
rem -------------------------------------------------------------------------
:start_tc_prompt
if exist %SPLM_SHR_DIR%\%ORACLE_SID%data\%OS_VERSION% set TC_DATA=%SPLM_SHR_DIR%\%ORACLE_SID%data\%OS_VERSION%
%TC_DATA%\tc_profilevars
cd /D %TC_TMP_DIR%

goto end

:start_portal_client
if /i "%SIDT_PAR1%" == "de" set TC_LANG=de_DE
if /i "%SIDT_PAR1%" == "en" set TC_LANG=en_US
rem if /i "%SIDT_PAR1%" == "en" set TC_LANG=de_DE
if /i "%SIDT_PAR1%" == "fr" set TC_LANG=fr_FR
if /i "%SIDT_PAR1%" == "it" set TC_LANG=it_IT
if /i "%SIDT_PAR1%" == "es" set TC_LANG=es_ES

::call %SPLM_SHR_DIR%\tcconfig\%BT_ENV%\portal\portal_4tier.bat  -nl %TC_LANG%
call %TC_ROOT%\portal\portal_4tier.bat  -nl %TC_LANG%
rem call %PORTAL_ROOT_DIR%\portal.bat -nl %TC_LANG%

goto end


:start_help
rem -------------------------------------------------------------------------------------
rem Helptext
rem -------------------------------------------------------------------------------------
cls
echo Software Implementation Design Team @ Siemens PLM Software
echo.
echo Syntax to start applications:
echo.
echo %SPLM_SHR_DIR%\start_apps\%OS_SYSTEM%\start_nx100.bat -v arg1 arg2 [arg3 arg4]
echo.
echo -v                        shows the version number of the SIDT Release
echo.
echo Argument 1:
echo de                        starting the application with german menus
echo en                        starting the application with english menus
echo fr                        starting the application with french menus
echo it                        starting the application with italian menus
echo es                        starting the application with spanish menus
echo.
echo Argument 2:
echo nx                        starting NX
echo nx_viewer                 starting NX Viewer
echo nx_prompt                 starting NX Command Prompt
echo kf_ice                    starting NX Knowledge Fusion ICE
echo ugopen_grip               starting NX Open Grip
echo vre                       starting NX Validation Rule Editor
echo pts                       starting NX Product Template Studio
echo qdb                       starting NX Quality Dashboard
echo plotadmin                 starting NX PLOT Admin
echo nxflexlm                  starting NX Flex Licensing
echo flex_server               starting Flexlm LMTools License Server
echo flex_client               starting Flexlm LMTools Client
echo ascmdf                    starting Ascmdf
echo cls2clf                   starting Cls2clf
echo gpm                       starting GPM
echo mkeditor                  starting Machining Knowledge Editor
echo mdfg                      starting Mdfg
echo nxpost                    starting Ugpost
echo xlator                    starting Xlator
echo postbuild                 starting Post Builder
echo catiav4                   starting CATIA V4 Translator
echo catiav5                   starting CATIA V5 Translator
echo dxfdwg                    starting DXFDWG Translator
echo iges                      starting IGES Translator
echo nxproe                    starting NXPROE Translator
echo step203ug                 starting STEP203UG Translator
echo step214ug                 starting STEP214UG Translator
echo ugto2d                    starting UGTO2D Translator
echo vda2ug                    starting VDATOUG
echo ug2vda                    starting UGTOVDA
echo tcvis10_lic_util          starting TcVis 10 License Borrow Utility
echo tcvis10_iseries_base      starting TcVis 10 iSeries Base
echo tcvis10_iseries_pro       starting TcVis 10 iSeries Professional
echo tcvis10_iseries_std       starting TcVis 10 iSeries Standard
echo tcvis10_base              starting TcVis 10 Base
echo tcvis10_std               starting TcVis 10 Standard
echo tcvis10_pro               starting TcVis 10 Professional
echo tcvis10_mockup            starting TcVis 10 Mockup
echo tcvis10_print             starting TcVis 10 Print
echo tcvis10_convert           starting TcVis 10 Convert
echo tcvis11_lic_util          starting TcVis 11 License Borrow Utility
echo tcvis11_iseries_base      starting TcVis 11 iSeries Base
echo tcvis11_iseries_pro       starting TcVis 11 iSeries Professional
echo tcvis11_iseries_std       starting TcVis 11 iSeries Standard
echo tcvis11_base              starting TcVis 11 Base
echo tcvis11_std               starting TcVis 11 Standard
echo tcvis11_pro               starting TcVis 11 Professional
echo tcvis11_mockup            starting TcVis 11 Mockup
echo tcvis11_print             starting TcVis 11 Print
echo tcvis11_convert           starting TcVis 11 Convert
echo tc_prompt                 starting Teamcenter TC UA Command Prompt
echo portal_client             starting Teamcenter UA Rich client



echo.
echo Argument 3:
echo db01                      starting Teamcenter Rich client or NX-Manager for this ORACLE_SID
echo.
echo Argument 4:
echo portal_client             Teamcenter 10.1 Rich client

goto end


:check_version
%SPLM_SHR_DIR%\start_apps\%OS_SYSTEM%\tools\msgbox.exe "SIDT Release for NX 10.0 / V1.02" "Software Implementation Design Team @ Siemens PLM Software"

:end

echo ---------------------------------------------------
echo End source start_NX100.bat
echo ---------------------------------------------------
