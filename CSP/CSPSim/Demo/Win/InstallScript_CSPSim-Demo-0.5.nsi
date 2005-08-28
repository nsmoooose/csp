; Combat Simulator Project - FlightSim Demo
; Copyright (C) 2002 The Combat Simulator Project
; http://csp.sourceforge.net
; 
; This program is free software; you can redistribute it and/or
; modify it under the terms of the GNU General Public License
; as published by the Free Software Foundation; either version 2
; of the License, or (at your option) any later version.
; 
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
; 
; You should have received a copy of the GNU General Public License
; along with this program; if not, write to the Free Software
; Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

;**
;* @file InstallScript.nsi
;*
;**/
;--------------------------------
;Define application defines
!define APPNAME "CSPSim Demo"
!define APPNAMEANDVERSION "${APPNAME} 0.5"
!define ALL_USERS	;used for environment vars

;--------------------------------
;Variables
Var PYTHONINSTPATH

;--------------------------------
;Configuration
!include "MUI.nsh"
;!include "CheckRegistryKey.nsi"
;!include "SplitFirstStringPartSpace.nsi"
;!include "SplitFirstStringPartSlash.nsi"
;!include "AddToPath.nsi"
;!include "SearchString.nsi"

!define MUI_ABORTWARNING
!define MUI_FINISHPAGE_RUN "$INSTDIR\bin\CSPSim.exe"
!define MUI_FINISHPAGE_RUN_NOTCHECKED
!define MUI_FINISHPAGE_TEXT "View README for more info, or visit http://csp.sourceforge.net"
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "CSPlogo.bmp"
!define MUI_HEADERIMAGE_UNBITMAP "CSPlogo.bmp"

;Install properties
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "csp_license.txt"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

;Uninstall properties
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

;Language
!insertmacro MUI_LANGUAGE "English"
  
;General Settings
OutFile "Setup.exe"
Name "${APPNAMEANDVERSION}"
InstallDir "$PROGRAMFILES\CSPSim"
XPStyle on
BrandingText "CSP - The Combat Simulator Project"

;We won't use this setting for releases
;ToDo: Recompile NSIS with NSIS_CONFIG_LOG set in config.h,
;otherwise logging won't be available
;LogSet on

;License page
LicenseData "./CSPSim-Demo-0.5/COPYING"
LicenseForceSelection radiobuttons 

;Show install details.
ShowInstDetails show

;--------------------------------
;Installer Sections

Section "CSPSim Core" SecCSPSimCore

	SectionIn 1 2 RO
  SetOutPath "$INSTDIR"
	
	;Add files to archive
	File /r "CSPSim-Demo-0.5\*.*"
;	File "CSPSim.dsw"	
	
	;Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\CSPSim "Install_Dir" "$INSTDIR"
	
 	;Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\CSPSim" "DisplayName" "CSPSim (remove only)"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\CSPSim" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteUninstaller "uninstall.exe"
	
	IfErrors +1 +3
		Abort "Installation of the CSPSim files failed. Aborting installation"
	
SectionEnd

; optional Start menu shortcut section
Section "Start Menu Shortcuts" SecStartMenuShortcuts
	
	SectionIn 1 2
  CreateDirectory "$SMPROGRAMS\CSPSim"
	SetOutPath "$INSTDIR\bin"
  CreateShortCut "$SMPROGRAMS\CSPSim\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\CSPSim\CSPSim.lnk" "$INSTDIR\bin\CSPSim.exe" "" "$INSTDIR\bin\CSPSim.exe" 0
SectionEnd

; optional Desktop shortcut section
Section "Desktop Shortcut" SecDesktopShortcut
  
	SectionIn 1 2
	; For past users, cleanup previous icon if still on desktop
	Delete "$DESKTOP\CSPSim.lnk"
	SetOutPath "$INSTDIR\bin"
;  CreateShortCut "$DESKTOP\CSPSim.lnk" "$INSTDIR\bin\CSPSim.exe" "" "$INSTDIR\bin\CSPSim.exe" 0
	CreateShortCut "$DESKTOP\CSPSim.lnk" "$INSTDIR\bin\CSPSim.exe" "" "D:\CSP\CSP\CSPSim\Demo\Win\CSPSim.ico"

SectionEnd

;--------------------------------
;Component Section Descriptions
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SecCSPSimCore} "CSPSim Demo (required)"
;	!insertmacro MUI_DESCRIPTION_TEXT ${SecCSPSimSources} "CSPSim Source Code"
;	!insertmacro MUI_DESCRIPTION_TEXT ${SecCSPSimMSVCSetup} "CSPSim MSVC/Python Setup (recommended for Development)"
	!insertmacro MUI_DESCRIPTION_TEXT ${SecStartMenuShortcuts} "CSPSim Start Menu Shortcuts"
	!insertmacro MUI_DESCRIPTION_TEXT ${SecDesktopShortcut} "CSPSim Desktop Shortcut"
;	!insertmacro MUI_DESCRIPTION_TEXT ${SecQuickLaunchShortcut} "CSPSim Quick Launch Shortcut"
!insertmacro MUI_FUNCTION_DESCRIPTION_END
 
;--------------------------------
;Uninstaller Section
Section "Uninstall"

;We're using this label to uninstall all files installed so far
;in case of an error during the installation process. It's called
;by the individual sections
;uninstall:

	; Delete all the files in the CSPSim folder.
	RMDir /r "$INSTDIR" 

	; Delete start menu stuff
  RMDir /r "$SMPROGRAMS\CSPSim"

  ; Delete desktop icons...
  Delete "$DESKTOP\CSPSim.lnk"
	
	; Delete quicklaunch icon
	Delete "$QUICKLAUNCH\CSPSim.lnk"

  ; Delete other shortcuts
  DeleteRegKey HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\CSPSim"
  DeleteRegKey HKEY_LOCAL_MACHINE "SOFTWARE\CSPSim"
  	
SectionEnd

; eof