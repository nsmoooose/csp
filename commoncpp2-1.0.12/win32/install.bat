@echo off
:: $Id: install.bat,v 1.1 2003/05/31 14:07:11 brandonb Exp $

if "%MSVCDir%" == "" goto error1
if "%MSDevDir%" == "" set MSDevDir="%MSVCDir%"
if "%DLL_PATH%" == "" set DLL_PATH="%MSDevDir%\Bin"

set _nul_=nul
IF "%OS%" == "Windows_NT" set _nul_=

:: Copy all DLL in a directory in path
if not exist "%DLL_PATH%\%_nul_%" mkdir "%DLL_PATH%"
copy debug\ccgnu2d.dll  "%DLL_PATH%"
copy debug\ccext2d.dll  "%DLL_PATH%"
copy release\ccgnu2.dll "%DLL_PATH%"
copy release\ccext2.dll "%DLL_PATH%"

:: Copy all include headers
if not exist "%MSVCDir%\Include\cc++2/%_nul_%" mkdir "%MSVCDir%\Include\cc++2"
if not exist "%MSVCDir%\Include\cc++2\cc++/%_nul_%" mkdir "%MSVCDir%\Include\cc++2\cc++"
if exist "%MSVCDir%\Include\cc++2\cc++\*.h" del "%MSVCDir%\Include\cc++2\cc++\*.h"
copy "cc++\config.h"   "%MSVCDir%\Include\cc++2\cc++"
copy "..\include\cc++\*.h" "%MSVCDir%\Include\cc++2\cc++"
copy "..\template\*.h" "%MSVCDir%\Include\cc++2\cc++"

:: Copy library files
if not exist "%MSVCDir%\lib\%_nul_%" mkdir "%MSVCDir%\lib"
copy debug\ccgnu2d.lib   "%MSVCDir%\lib"
copy debug\ccext2d.lib   "%MSVCDir%\lib"
copy release\ccgnu2.lib  "%MSVCDir%\lib"
copy release\ccext2.lib  "%MSVCDir%\lib"

goto end

:error1
echo The "MSVCDIR" environment variable has not been defined.
echo Typically this is C:\Program Files\Microsoft Visual Studio\VC98
echo.
echo For Windows 95/98/ME users:
echo   Select Start/Run
echo Type in:
echo   notepad C:\AUTOEXEC.BAT
echo and add the following line:
echo   SET MSVCDIR=C:\Program Files\Microsoft Visual Studio\VC98
echo Change the above to suit your system, as needed.
echo.
echo For Windows NT/2000/XP users:
echo   Select Start/Settings/Control Panel/System/Advanced/Environment Variables/System Variables
echo.
goto end

:end
