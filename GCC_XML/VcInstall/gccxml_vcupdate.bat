@echo off
rem --------------------------------------------------------------------------
rem This is a driver script to update GCC-XML's support of new Visual Studio
rem installations.
rem --------------------------------------------------------------------------

rem Reference the TEMP directory in a safe way.
set vc_temp=%TEMP%.

rem Get the current working directory.
type %0\..\..\VcInstall\vc_helper > "%vc_temp%\vc_env_temp.bat"
cd >>"%vc_temp%\vc_env_temp.bat"
call "%vc_temp%\vc_env_temp.bat"
del "%vc_temp%\vc_env_temp.bat"
set cwd=%vc_result%

rem Change to the directory containing this script and get its location.
type %0\..\..\VcInstall\vc_helper > "%vc_temp%\vc_env_temp.bat"
cd /D "%0\..\.."
cd >>"%vc_temp%\vc_env_temp.bat"
call "%vc_temp%\vc_env_temp.bat"
del "%vc_temp%\vc_env_temp.bat"
set vc_dir=%vc_result%

rem Change back to the original working directory.
cd /D "%cwd%"

rem Run the installer executable to do the rest of the work.
"%vc_dir%\VcInstall\vcInstall" "%vc_dir%\VcInstall" "%vc_dir%\bin"

if errorlevel 1 pause
