@ECHO OFF

if exist "%ProgramFiles%\3Dconnexion\3Dconnexion 3DxWare\3dxsrv.exe" set DriverExe="%ProgramFiles%\3Dconnexion\3Dconnexion 3DxWare\3dxsrv.exe"
if exist "%ProgramFiles(x86)%\3Dconnexion\3Dconnexion 3DxWare\3dxsrv.exe" set DriverExe="%ProgramFiles(x86)%\3Dconnexion\3Dconnexion 3DxWare\3dxsrv.exe"
if exist "%ProgramFiles%\3Dconnexion\3Dconnexion 3DxSoftware\3DxWare64\3dxsrv.exe" set DriverExe="%ProgramFiles%\3Dconnexion\3Dconnexion 3DxSoftware\3DxWare64\3dxsrv.exe"
if exist "%ProgramFiles(x86)%\3Dconnexion\3Dconnexion 3DxSoftware\3DxWare64\3dxsrv.exe" set DriverExe="%ProgramFiles(x86)%\3Dconnexion\3Dconnexion 3DxSoftware\3DxWare64\3dxsrv.exe"

if exist "%ProgramFiles%\3Dconnexion\3Dconnexion 3DxSoftware\3DxWare64\3dxsrv.exe" set DriverExe="%ProgramFiles%\3Dconnexion\3Dconnexion 3DxSoftware\3DxWare64\3dxsrv.exe"
if exist "%ProgramFiles(x86)%\3Dconnexion\3Dconnexion 3DxSoftware\3DxWare64\3dxsrv.exe" set DriverExe="%ProgramFiles(x86)%\3Dconnexion\3Dconnexion 3DxSoftware\3DxWare64\3dxsrv.exe"


echo.
echo DriverExe___%DriverExe% %1
echo.
start /min "3Dconnexion" %DriverExe% %1

rem pause