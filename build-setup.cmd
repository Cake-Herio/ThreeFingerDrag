@echo off
setlocal
cd /d "%~dp0"
if not defined VSINSTALLDIR (
  for /f "usebackq delims=" %%V in (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do call "%%V\Common7\Tools\VsDevCmd.bat" -arch=x64
)
if not defined VSINSTALLDIR exit /b 1
set "TOOLSET=v143"
if exist "%VSINSTALLDIR%MSBuild\Microsoft\VC\v180\Platforms\x64\PlatformToolsets\v145" set "TOOLSET=v145"
MSBuild.exe ThreeFingerDrag.sln /t:Rebuild /m /p:Configuration=Release /p:Platform=x64 /p:PlatformToolset=%TOOLSET% "/p:ForceImportBeforeCppTargets=%~dp0packaging\static-runtime.props"
if errorlevel 1 exit /b 1
if defined ISCC (
  "%ISCC%" inno_script.iss
) else (
  ISCC.exe inno_script.iss
)
exit /b %errorlevel%
