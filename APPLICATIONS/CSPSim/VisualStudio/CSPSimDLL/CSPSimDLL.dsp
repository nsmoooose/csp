# Microsoft Developer Studio Project File - Name="CSPSimDLL" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=CSPSimDLL - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "CSPSimDLL.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "CSPSimDLL.mak" CFG="CSPSimDLL - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CSPSimDLL - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "CSPSimDLL - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "CSPSimDLL - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CSPSIMDLL_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "../../Include" /I "../../../SimData/Include" /I "../../../../THIRDPARTYLIBS/include" /I "../../../../THIRDPARTYLIBS/Demeter" /I "C:/Python22/include" /I "../../SimData/include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CSPSIMDLL_EXPORTS" /D "_STLP_USE_OWN_NAMESPACE" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 osgText.lib DemeterVisualC6.lib opengl32.lib SDL.lib _cSimData.lib osgParticle.lib osgUtil.lib osgDB.lib osg.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /pdb:none /machine:I386 /out:"../../Bin/_CSP.dll" /libpath:"../../../../THIRDPARTYLIBS/lib" /libpath:"C:/Python22/libs"
# SUBTRACT LINK32 /debug

!ELSEIF  "$(CFG)" == "CSPSimDLL - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CSPSIMDLL_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /Zi /Od /I "../../SimData/include" /I "../../../THIRDPARTYLIBS/demeter" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CSPSIMDLL_EXPORTS" /D "_STLP_USE_OWN_NAMESPACE" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /nodefaultlib:"msvcrt.lib" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "CSPSimDLL - Win32 Release"
# Name "CSPSimDLL - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\Source\AeroDynamics.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\AircraftModel.cpp

!IF  "$(CFG)" == "CSPSimDLL - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "CSPSimDLL - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\Source\AircraftObject.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\base.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\BaseScreen.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\Colorspace.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\Config.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\CSP_wrap.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\CSPSim.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\DynamicalSystem.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\DynamicObject.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\EventMapIndex.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\EventMapping.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\F16Model.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\GameScreen.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\HID.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\InputInterface.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\LogoScreen.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\MenuScreen.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\Message.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\ObjectModel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\ObjectRangeInfo.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\Platform.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\ScreenInfo.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\ScreenInfoManager.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\SimObject.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\SimpleConfig.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\sky.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\SmokeEffects.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\StaticObject.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\TankObject.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\TerrainObject.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\Tools.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\trees.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\VirtualBattlefield.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\Include\AeroDynamics.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\AircraftObject.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\BaseController.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\BasePhysics.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\BaseScreen.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\Colorspace.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\compiler.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\Config.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\CSPSim.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\debug_types.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\DynamicObject.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\EventMapIndex.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\EventMapping.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\FLCS.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\GameScreen.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\GlibCsp.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\HID.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\InputInterface.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\LogoScreen.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\LogStream.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\MenuScreen.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\Message.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\ObjectModel.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\ObjectRangeInfo.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\OGLText.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\Platform.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\ScreenInfo.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\ScreenInfoManager.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\SimObject.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\SimpleConfig.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\SmokeEffects.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\StaticObject.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\stdinc.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\TankObject.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\TerrainObject.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\Tools.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\VehicleModel.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\VirtualBattlefield.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=..\..\Source\CSP.i

!IF  "$(CFG)" == "CSPSimDLL - Win32 Release"

# Begin Custom Build
InputPath=..\..\Source\CSP.i
InputName=CSP

"..\..\Source\$(InputName)_wrap.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	C:\progra~1\SWIG-1.3.17\swig -c++ -python -noexcept -DWIN32 -I..\..\Include -o ..\..\Source\$(InputName)_wrap.cpp $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "CSPSimDLL - Win32 Debug"

# Begin Custom Build
InputPath=..\..\Source\CSP.i
InputName=CSP

"..\..\Source\$(InputName)_wrap.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	C:\progra~1\SWIG-1.3.17\swig -c++ -python -noexcept -DWIN32 -I..\Include -o ..\..\Source\$(InputName)_wrap.cpp $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# End Target
# End Project
