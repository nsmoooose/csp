; Combat Simulator Project
; Windows layout installer script.

; Requires one externally defined variable:
;   VERSION - the layout version (e.g. "0.6").  The layout directory
;             is expected to be csp-layout-${VERSION}.

; Modern UI
  !include "MUI.nsh"

  !define WriteEnvStr_RegKey 'HKCU "Environment"'

  Name "CSP Layout ${VERSION}"
  OutFile "csp-layout-${VERSION}-installer.exe"

  InstallDir "$PROGRAMFILES\csp-layout-${VERSION}"
  InstallDirRegKey HKCU "Software\csp-layout-${VERSION}" ""

  Var MUI_TEMP
  Var STARTMENU_FOLDER

  !define MUI_HEADERIMAGE
  !define MUI_HEADERIMAGE_BITMAP "logo.bmp"
  !define MUI_ABORTWARNING

  !define MUI_FINISHPAGE_RUN
  !define MUI_FINISHPAGE_RUN_TEXT "Run layout editor"
  !define MUI_FINISHPAGE_RUN_FUNCTION LaunchLink
  !define MUI_FINISHPAGE_RUN_NOTCHECKED
  !define MUI_FINISHPAGE_SHOWREADME "$INSTDIR\README.txt"
  !define MUI_FINISHPAGE_SHOWREADME_TEXT "View release notes"
  !define MUI_FINISHPAGE_SHOWREADME_NOTCHECKED
  !define MUI_FINISHPAGE_LINK "Online release notes and support"
  !define MUI_FINISHPAGE_LINK_LOCATION "http://csp.sf.net/wiki/Windows_Demo_0.6"
  !define MUI_FINISHPAGE_NOREBOOTSUPPORT

  !insertmacro MUI_PAGE_LICENSE "csp-layout-${VERSION}\COPYING.txt"
  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_STARTMENU Application $STARTMENU_FOLDER
  !insertmacro MUI_PAGE_INSTFILES
  !insertmacro MUI_PAGE_FINISH

  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES

  !insertmacro MUI_LANGUAGE "English"

  VIProductVersion "${VERSION}.0.0"
  VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" "Combat Simulator Project Layout Editor"
  VIAddVersionKey /LANG=${LANG_ENGLISH} "Comments" "Visit http://csp.sf.net/wiki/Demo for more information."
  VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "CSP Layout Installer"
  VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "${VERSION}"

Section "Layout" SecLayout

  SetOutPath "$INSTDIR"

  File /r "csp-layout-${VERSION}\*"

  ;Store installation folder
  WriteRegStr HKCU "Software\csp-layout-${VERSION}" "" $INSTDIR

  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"

  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    CreateDirectory "$SMPROGRAMS\$STARTMENU_FOLDER"
    CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
    CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\readme.lnk" "$INSTDIR\README.txt"
    SetOutPath "$INSTDIR\bin"
    CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\layout.lnk" "$INSTDIR\bin\layout.exe" "" "" "" "" "" "Run layout editor"
    CreateShortCut "$INSTDIR\layout.lnk" "$INSTDIR\bin\layout.exe" "" "" "" "" "" "Run layout editor"
    SetOutPath "$INSTDIR"
  !insertmacro MUI_STARTMENU_WRITE_END

SectionEnd

Section "Uninstall"

  RMDir /r "$INSTDIR"

  !insertmacro MUI_STARTMENU_GETFOLDER Application $MUI_TEMP
  Delete "$SMPROGRAMS\$MUI_TEMP\Uninstall.lnk"
  Delete "$SMPROGRAMS\$MUI_TEMP\layout.lnk"
  Delete "$SMPROGRAMS\$MUI_TEMP\readme.lnk"
  StrCpy $MUI_TEMP "$SMPROGRAMS\$MUI_TEMP"
  startMenuDeleteLoop:
    ClearErrors
    RMDir $MUI_TEMP
    GetFullPathName $MUI_TEMP "$MUI_TEMP\.."
    IfErrors startMenuDeleteLoopDone
    StrCmp $MUI_TEMP $SMPROGRAMS startMenuDeleteLoopDone startMenuDeleteLoop
  startMenuDeleteLoopDone:

  DeleteRegKey /ifempty HKCU "Software\csp-layout-${VERSION}"

SectionEnd

Function LaunchLink
  ExecShell "" "$INSTDIR\layout.lnk"
FunctionEnd

