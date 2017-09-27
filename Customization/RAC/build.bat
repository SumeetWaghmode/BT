@echo off
setlocal

set TC_ROOT=N:\ugs\tc11\11
set JAVA_HOME=X:\Java745\JDK

set BASE_DIR=O:\0SBT-PR-TC11UPGR\trunk\MRT
set ANT_HOME=%BASE_DIR%/ant/home/apache-ant-1.8.0
set PATH=%PATH%;%ANT_HOME%\bin

if not defined TC_ROOT (
	echo "TC_ROOT is not defined"
	exit /b 1
)

if not exist %TC_ROOT%\portal (
	echo "Improper Teamcenter installation. Access to %TC_ROOT%\portal\plugins directory required"
	exit /b 1
)

if not defined TC_BUILD (
	set TC_BUILD=%TEMP%
)

echo "The result of the compilation would be in [%TC_BUILD%]"
set CONFIGURATION_DIR=D:/JAR
set ANT_INCLUDE=%BASE_DIR%/ant/include

call ant -q -lib %ANT_INCLUDE%/ant-contrib-1.0b3.jar -lib %ANT_INCLUDE%/jag-contrib_1.0.0.jar -DTC_ROOT=%TC_ROOT% -Dbuild.dir=%TEMP%\sbt\build -Dpackage.dir=%CONFIGURATION_DIR% create:all

rem call ant -q -lib %ANT_INCLUDE%/ant-contrib-1.0b3.jar -lib %ANT_INCLUDE%/jag-contrib_1.0.0.jar -DTC_ROOT=%TC_ROOT% -Dbuild.dir=%TEMP%\sbt\build -Dpackage.dir=%CONFIGURATION_DIR% create:all


pause