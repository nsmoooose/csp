# Microsoft Developer Studio Project File - Name="CSPFlightSim" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=CSPFlightSim - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "CSPFlightSim.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "CSPFlightSim.mak" CFG="CSPFlightSim - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CSPFlightSim - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "CSPFlightSim - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "CSPFlightSim - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GR /GX /Zi /Od /Ob2 /I "../Include" /I "../../../THIRDPARTYLIBS/Include" /I "../../../THIRDPARTYLIBS/demeter" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_STLP_USE_OWN_NAMESPACE" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 osgparticle.lib osgtext.lib demetervisualc6.lib osg.lib osgUtil.lib osgDb.lib SDL.lib SDLMain.lib opengl32.lib glaux.lib glu32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib DemeterVisualC6.lib SDL_Console.lib gdal_i.lib /nologo /subsystem:windows /debug /machine:I386 /out:"../Bin/CSPFlightSim_Release.exe" /pdbtype:sept /libpath:"../../../THIRDPARTYLIBS/lib"
# SUBTRACT LINK32 /map

!ELSEIF  "$(CFG)" == "CSPFlightSim - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /Zi /Od /I "..\Include" /I "..\..\..\THIRDPARTYLIBS\Include" /I "..\..\..\THIRDPARTYLIBS\demeter" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_STLP_USE_OWN_NAMESPACE" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 osgParticled.lib gdal_i.lib osgd.lib osgDBd.lib SDL_console.lib SDLd.lib opengl32.lib osgTextd.lib DemeterVisualC6d.lib osgUtild.lib SDLmaind.lib /nologo /subsystem:windows /incremental:no /pdb:"../DemoPackage/CSPFlightSim_Debug.pdb" /debug /machine:I386 /nodefaultlib:"msvcrt.lib" /out:"../Bin/CSPFlightSim_Debug.exe" /pdbtype:sept /libpath:"..\..\..\THIRDPARTYLIBS\lib"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "CSPFlightSim - Win32 Release"
# Name "CSPFlightSim - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\Source\AAAController.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\AAAObject.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\AeroParam.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\AeroParamSymbol.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\AIAirplaneController.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\AirplaneController.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\AirplaneInput.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\AirplaneObject.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\AirplanePhysics.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\base.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\BaseController.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\BaseObject.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\BaseSymbol.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\CockpitDrawable.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Config.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\ConsoleCommands.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\CSPFlightSim.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\DirVectorDrawable.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\GameScreen.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\global.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\GlobalCommands.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Hud.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\HudElement.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\HudGeometricElement.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\HudTextElement.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\InstantActionController.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Source\InstantActionManager.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Source\LogoScreen.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\logstream.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\main.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\MenuScreen.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Message.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\MessageQueue.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\MissileController.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\MissileObject.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\ObjectFactory.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\ObjectRangeInfo.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\ObjectSymbol.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\ObjectTypeSymbol.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\ParticleObject.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Platform.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\ProjectileController.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\ProjectileObject.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\SimTime.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\sky.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\StaticObject.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\stdinc.cpp

!IF  "$(CFG)" == "CSPFlightSim - Win32 Release"

!ELSEIF  "$(CFG)" == "CSPFlightSim - Win32 Debug"

# ADD CPP /Yc"stdinc.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Source\SymbolTable.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\TankController.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\TankObject.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\TerrainObject.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\TerrainSymbol.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Tools.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\trees.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\TypesMath.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\TypesMatrix3.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\TypesMatrix4.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\TypesQuaternion.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\TypesVector3.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\TypesVector4.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\VirtualBattlefield.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\VirtualBattlefieldScene.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\Include\AAAController.h
# End Source File
# Begin Source File

SOURCE=..\Include\AAAObject.h
# End Source File
# Begin Source File

SOURCE=..\Include\AeroParam.h
# End Source File
# Begin Source File

SOURCE=..\Include\AeroParamSymbol.h
# End Source File
# Begin Source File

SOURCE=..\Include\AIAirplaneController.h
# End Source File
# Begin Source File

SOURCE=..\Include\AirplaneController.h
# End Source File
# Begin Source File

SOURCE=..\Include\AirplaneInput.h
# End Source File
# Begin Source File

SOURCE=..\Include\AirplaneObject.h
# End Source File
# Begin Source File

SOURCE=..\Include\AirplanePhysics.h
# End Source File
# Begin Source File

SOURCE=..\Include\BaseController.h
# End Source File
# Begin Source File

SOURCE=..\Include\BaseInput.h
# End Source File
# Begin Source File

SOURCE=..\Include\BaseObject.h
# End Source File
# Begin Source File

SOURCE=..\Include\BasePhysics.h
# End Source File
# Begin Source File

SOURCE=..\Include\BaseScreen.h
# End Source File
# Begin Source File

SOURCE=..\Include\BaseSymbol.h
# End Source File
# Begin Source File

SOURCE=..\Include\CockpitDrawable.h
# End Source File
# Begin Source File

SOURCE=..\Include\compiler.h
# End Source File
# Begin Source File

SOURCE=..\..\..\THIRDPARTYLIBS\include\CON_console.h
# End Source File
# Begin Source File

SOURCE=..\Include\Config.h
# End Source File
# Begin Source File

SOURCE=..\Include\ConsoleCommands.h
# End Source File
# Begin Source File

SOURCE=..\Include\CSPFlightSim.h
# End Source File
# Begin Source File

SOURCE=..\Include\debug_types.h
# End Source File
# Begin Source File

SOURCE=..\Include\DirVectorDrawable.h
# End Source File
# Begin Source File

SOURCE=..\Include\GameScreen.h
# End Source File
# Begin Source File

SOURCE=..\Include\global.h
# End Source File
# Begin Source File

SOURCE=..\Include\GlobalCommands.h
# End Source File
# Begin Source File

SOURCE=..\Include\Hud.h
# End Source File
# Begin Source File

SOURCE=..\Include\HudElement.h
# End Source File
# Begin Source File

SOURCE=..\Include\HudGeometricElement.h
# End Source File
# Begin Source File

SOURCE=..\Include\HudTextElement.h
# End Source File
# Begin Source File

SOURCE=..\Include\InstantActionController.h
# End Source File
# Begin Source File

SOURCE=..\Include\InstantActionManager.h
# End Source File
# Begin Source File

SOURCE=..\Include\LogoScreen.h
# End Source File
# Begin Source File

SOURCE=..\Include\LogStream.h
# End Source File
# Begin Source File

SOURCE=..\Include\MenuScreen.h
# End Source File
# Begin Source File

SOURCE=..\Include\Message.h
# End Source File
# Begin Source File

SOURCE=..\Include\MessageQueue.h
# End Source File
# Begin Source File

SOURCE=..\Include\MissileController.h
# End Source File
# Begin Source File

SOURCE=..\Include\MissileObject.h
# End Source File
# Begin Source File

SOURCE=..\Include\ObjectFactory.h
# End Source File
# Begin Source File

SOURCE=..\Include\ObjectRangeInfo.h
# End Source File
# Begin Source File

SOURCE=..\Include\ObjectSymbol.h
# End Source File
# Begin Source File

SOURCE=..\Include\ObjectTypeSymbol.h
# End Source File
# Begin Source File

SOURCE=..\Include\ParticleObject.h
# End Source File
# Begin Source File

SOURCE=..\Include\Platform.h
# End Source File
# Begin Source File

SOURCE=..\Include\ProjectileController.h
# End Source File
# Begin Source File

SOURCE=..\Include\ProjectileObject.h
# End Source File
# Begin Source File

SOURCE=..\Include\SimTime.h
# End Source File
# Begin Source File

SOURCE=..\Include\StaticObject.h
# End Source File
# Begin Source File

SOURCE=..\Include\stdinc.h
# End Source File
# Begin Source File

SOURCE=..\Include\SymbolTable.h
# End Source File
# Begin Source File

SOURCE=..\Include\TankController.h
# End Source File
# Begin Source File

SOURCE=..\Include\TankObject.h
# End Source File
# Begin Source File

SOURCE=..\Include\TerrainObject.h
# End Source File
# Begin Source File

SOURCE=..\Include\TerrainSymbol.h
# End Source File
# Begin Source File

SOURCE=..\Include\Tools.h
# End Source File
# Begin Source File

SOURCE=..\Include\TypesMath.h
# End Source File
# Begin Source File

SOURCE=..\Include\TypesMath.inl
# End Source File
# Begin Source File

SOURCE=..\Include\TypesMatrix3.h
# End Source File
# Begin Source File

SOURCE=..\Include\TypesMatrix4.h
# End Source File
# Begin Source File

SOURCE=..\Include\TypesQuaternion.h
# End Source File
# Begin Source File

SOURCE=..\Include\TypesVector3.h
# End Source File
# Begin Source File

SOURCE=..\Include\TypesVector3.inl
# End Source File
# Begin Source File

SOURCE=..\Include\TypesVector4.h
# End Source File
# Begin Source File

SOURCE=..\Include\TypesVector4.inl
# End Source File
# Begin Source File

SOURCE=..\Include\VirtualBattlefield.h
# End Source File
# Begin Source File

SOURCE=..\Include\VirtualBattlefieldScene.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
