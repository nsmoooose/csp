# Microsoft Developer Studio Project File - Name="CSPSimAppli" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=CSPSimAppli - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "CSPSimAppli.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "CSPSimAppli.mak" CFG="CSPSimAppli - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CSPSimAppli - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "CSPSimAppli - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "CSPSimAppli - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "../../Include" /I "../../../SimData/Include" /I "../../../../THIRDPARTYLIBS/include" /I "../../../../THIRDPARTYLIBS/Demeter" /I "C:/Python22/include" /I "../../SimData/include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 SDLmain.lib osgText.lib DemeterVisualC6.lib opengl32.lib SDL.lib _cSimData.lib osgParticle.lib osgUtil.lib osgDB.lib osg.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /pdb:none /machine:I386 /out:"../../Bin/CSPSim.exe" /libpath:"C:/Python22/libs" /libpath:"../../../../THIRDPARTYLIBS/lib"

!ELSEIF  "$(CFG)" == "CSPSimAppli - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../Bin"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /Zi /Od /I "../../Include" /I "../../../SimData/Include" /I "../../../../THIRDPARTYLIBS/include" /I "../../../../THIRDPARTYLIBS/Demeter" /I "C:/Python22/include" /I "../../SimData/include" /I "../../../THIRDPARTYLIBS/demeter" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FAs /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 DemeterVisualC6d.lib opengl32.lib SDLmain.lib SDLd.lib _cSimDatad.lib osgTextd.lib osgParticled.lib osgUtild.lib osgDBd.lib osgd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /debug /machine:I386 /nodefaultlib:"msvcrt.lib" /out:"../../Bin/CSPSimd.exe" /pdbtype:sept /libpath:"C:/Python22/libs" /libpath:"../../../../THIRDPARTYLIBS/lib"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "CSPSimAppli - Win32 Release"
# Name "CSPSimAppli - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\Source\AeroDynamics.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\AircraftModel.cpp
# PROP Exclude_From_Build 1
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

SOURCE=..\..\Source\LogStream.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\main.cpp
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

SOURCE=..\..\Include\DirVectorDrawable.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\DynamicalSystem.h
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

SOURCE=..\..\Include\Sky.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\SmokeEffects.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\StaticObject.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\TankObject.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\TerrainObject.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\TerrainSymbol.h
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

!IF  "$(CFG)" == "CSPSimAppli - Win32 Release"

# Begin Custom Build
InputPath=..\..\Source\CSP.i
InputName=CSP

"..\..\Source\$(InputName)_wrap.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	C:\progra~1\SWIG-1.3.17\swig -c++ -python -noexcept -DWIN32 -I..\..\Include -o ..\..\Source\$(InputName)_wrap.cpp $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "CSPSimAppli - Win32 Debug"

# Begin Custom Build
InputPath=..\..\Source\CSP.i
InputName=CSP

"..\..\Source\$(InputName)_wrap.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	C:\progra~1\SWIG-1.3.17\swig -c++ -python -noexcept -DWIN32 -I..\..\Include -o ..\..\Source\$(InputName)_wrap.cpp $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# End Target
# End Project
