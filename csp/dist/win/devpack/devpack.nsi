; Combat Simulator Project
; Windows devpack installer script.

; Requires two externally defined variables:
;   SRC - the source directory for the headers and libraries in the devpack
;	      with the following subdirectories:
;            include/  - all headers (in subdirectories as appropriate)
;            bin/      - all exe and dll files (no subdirectories)
;            lib/      - all lib files (no subdirectories)
;   VERSION - the devpack version (e.g. "0.6")

  !define CSPDEVPACK 'CSPDEVPACK'

; Modern UI
  !include "MUI.nsh"

  !define WriteEnvStr_RegKey 'HKCU "Environment"'

  Name "CSP Devpack ${VERSION}"
  OutFile "cspdevpack-${VERSION}-installer.exe"

  InstallDir "$PROGRAMFILES\cspdevpack-${VERSION}"
  InstallDirRegKey HKCU "Software\cspdevpack-${VERSION}" ""

  !define MUI_HEADERIMAGE
  !define MUI_HEADERIMAGE_BITMAP "logo.bmp"
  !define MUI_ABORTWARNING

  !insertmacro MUI_PAGE_LICENSE "COPYING"
  ;!insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_INSTFILES

  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES

  !insertmacro MUI_LANGUAGE "English"

  VIProductVersion "${VERSION}.0.0"
  VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" "Combat Simulator Project Devpack"
  VIAddVersionKey /LANG=${LANG_ENGLISH} "Comments" "Visit http://csp.sf.net/wiki/Devpack for more information."
  VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "CSP Devpack Installer"
  VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "${VERSION}"


Section "Devpack" SecDevpack

  SetOutPath "$INSTDIR"
 
  File "README"
  File "COPYING"
  File /r "LICENSES"
  File /r "${SRC}\bin"
  File /r "${SRC}\lib"
  File /r "${SRC}\include"

  ;Store installation folder
  WriteRegStr HKCU "Software\cspdevpack-${VERSION}" "" $INSTDIR

  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"

  ;Set the CSPDEVPACK environment variable
  ClearErrors
  WriteRegExpandStr ${WriteEnvStr_RegKey} "${CSPDEVPACK}" $INSTDIR
  IfErrors 0 env_update_ok
  MessageBox MB_OK|MB_ICONINFORMATION "The ${CSPDEVPACK} environment variable could not be set for the current user."
  Goto env_update_end
 env_update_ok:
  SendMessage ${HWND_BROADCAST} ${WM_WININICHANGE} 0 "STR:Environment" /TIMEOUT=5000
 env_update_end:

SectionEnd

  ;Language strings
  ;LangString DESC_SecDummy ${LANG_ENGLISH} "A test section."

  ;Assign language strings to sections
  ;!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  ;  !insertmacro MUI_DESCRIPTION_TEXT ${SecDummy} $(DESC_SecDummy)
  ;!insertmacro MUI_FUNCTION_DESCRIPTION_END


Section "Uninstall"

  ;Delete "$INSTDIR\Uninstall.exe"

  RMDir /r "$INSTDIR"

  DeleteRegKey /ifempty HKCU "Software\cspdevpack-${VERSION}"

  DeleteRegValue ${WriteEnvStr_RegKey} "${CSPDEVPACK}"

SectionEnd
