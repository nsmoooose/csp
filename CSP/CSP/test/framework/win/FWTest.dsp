# Microsoft Developer Studio Project File - Name="FWTest" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=FWTest - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "FWTest.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "FWTest.mak" CFG="FWTest - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "FWTest - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "FWTest - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "FWTest - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\..\src\framework" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "DirectInput" /D "D3D" /D "DSound" /D "DirectMath" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 ws2_32.lib dsound.lib d3d8.lib dxguid.lib dinput8.lib winmm.lib d3dx8.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "FWTest - Win32 Debug"

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
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\src\framework" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "DirectInput" /D "D3D" /D "DSound" /D "DirectMath" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ws2_32.lib dsound.lib d3d8.lib dxguid.lib dinput8.lib winmm.lib d3dx8.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "FWTest - Win32 Release"
# Name "FWTest - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Framework_CPP"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\framework\audio\win\Audio_DX.cpp

!IF  "$(CFG)" == "FWTest - Win32 Release"

# ADD CPP /I "../../src/framework"

!ELSEIF  "$(CFG)" == "FWTest - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\framework\audio\AudioData.cpp

!IF  "$(CFG)" == "FWTest - Win32 Release"

# ADD CPP /I "../../src/framework"

!ELSEIF  "$(CFG)" == "FWTest - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\framework\graphics\win\D3D.cpp

!IF  "$(CFG)" == "FWTest - Win32 Release"

# ADD CPP /I "../../src/framework"

!ELSEIF  "$(CFG)" == "FWTest - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\framework\input\win\DirectInput.cpp

!IF  "$(CFG)" == "FWTest - Win32 Release"

# ADD CPP /I "../../src/framework"

!ELSEIF  "$(CFG)" == "FWTest - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\framework\audio\FactoryAudio.cpp

!IF  "$(CFG)" == "FWTest - Win32 Release"

# ADD CPP /I "../../src/framework"

!ELSEIF  "$(CFG)" == "FWTest - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\framework\graphics\FactoryGraphics.cpp

!IF  "$(CFG)" == "FWTest - Win32 Release"

# ADD CPP /I "../../src/framework"

!ELSEIF  "$(CFG)" == "FWTest - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\framework\input\FactoryInput.cpp

!IF  "$(CFG)" == "FWTest - Win32 Release"

# ADD CPP /I "../../src/framework"

!ELSEIF  "$(CFG)" == "FWTest - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\framework\standard\FactoryMath.cpp

!IF  "$(CFG)" == "FWTest - Win32 Release"

# ADD CPP /I "../../src/framework"

!ELSEIF  "$(CFG)" == "FWTest - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\framework\net\FactoryNetwork.cpp

!IF  "$(CFG)" == "FWTest - Win32 Release"

# ADD CPP /I "../../src/framework"

!ELSEIF  "$(CFG)" == "FWTest - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\framework\platform\FactoryPlatform.cpp

!IF  "$(CFG)" == "FWTest - Win32 Release"

# ADD CPP /I "../../src/framework"

!ELSEIF  "$(CFG)" == "FWTest - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\framework\standard\FrameworkError.cpp

!IF  "$(CFG)" == "FWTest - Win32 Release"

# ADD CPP /I "../../src/framework"

!ELSEIF  "$(CFG)" == "FWTest - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\framework\audio\NoAudio.cpp

!IF  "$(CFG)" == "FWTest - Win32 Release"

# ADD CPP /I "../../src/framework"

!ELSEIF  "$(CFG)" == "FWTest - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\framework\graphics\NoGraphics.cpp

!IF  "$(CFG)" == "FWTest - Win32 Release"

# ADD CPP /I "../../src/framework"

!ELSEIF  "$(CFG)" == "FWTest - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\framework\input\NoInput.cpp

!IF  "$(CFG)" == "FWTest - Win32 Release"

# ADD CPP /I "../../src/framework"

!ELSEIF  "$(CFG)" == "FWTest - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\framework\net\NoNetwork.cpp

!IF  "$(CFG)" == "FWTest - Win32 Release"

# ADD CPP /I "../../src/framework"

!ELSEIF  "$(CFG)" == "FWTest - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\framework\platform\NoPlatform.cpp

!IF  "$(CFG)" == "FWTest - Win32 Release"

# ADD CPP /I "../../src/framework"

!ELSEIF  "$(CFG)" == "FWTest - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\framework\platform\StandardPlatform.cpp

!IF  "$(CFG)" == "FWTest - Win32 Release"

# ADD CPP /I "../../src/framework"

!ELSEIF  "$(CFG)" == "FWTest - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\framework\net\windows\WindowsNetwork.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\framework\platform\win\WindowsPlatform.cpp

!IF  "$(CFG)" == "FWTest - Win32 Release"

# ADD CPP /I "../../src/framework"

!ELSEIF  "$(CFG)" == "FWTest - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\App.cpp

!IF  "$(CFG)" == "FWTest - Win32 Release"

# ADD CPP /I "../../src/framework"

!ELSEIF  "$(CFG)" == "FWTest - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Main.cpp

!IF  "$(CFG)" == "FWTest - Win32 Release"

# ADD CPP /I "../../src/framework"

!ELSEIF  "$(CFG)" == "FWTest - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Script1.rc
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "Framework_H"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\framework\audio\win\Audio_DX.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\framework\audio\AudioData.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\framework\graphics\win\D3D.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\framework\graphics\win\D3DMaterialObject.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\framework\graphics\win\D3DMaterialObject.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\framework\graphics\win\D3DPolygonListObject.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\framework\graphics\win\D3DPolygonListObject.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\framework\input\win\DirectInput.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\framework\standard\win\DirectMath.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\framework\audio\FactoryAudio.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\framework\graphics\FactoryGraphics.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\framework\input\FactoryInput.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\framework\standard\FactoryMath.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\framework\net\FactoryNetwork.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\framework\platform\FactoryPlatform.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\framework\standard\FrameworkError.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\framework\standard\License.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\framework\audio\NoAudio.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\framework\graphics\NoGraphics.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\framework\input\NoInput.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\framework\net\NoNetwork.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\framework\platform\NoPlatform.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\framework\app\StandardApp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\framework\audio\StandardAudio.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\framework\standard\StandardFramework.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\framework\graphics\StandardGraphics.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\framework\input\StandardInput.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\framework\standard\StandardMath.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\framework\net\StandardNetwork.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\framework\platform\StandardPlatform.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\framework\app\TypesApp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\framework\audio\TypesAudio.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\framework\standard\TypesError.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\framework\standard\TypesFramework.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\framework\graphics\TypesGraphics.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\framework\input\TypesInput.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\framework\standard\TypesMath.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\framework\net\TypesNetwork.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\framework\platform\TypesPlatform.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\framework\net\windows\WindowsNetwork.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\framework\platform\win\WindowsPlatform.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\App.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\icon1.ico
# End Source File
# End Group
# End Target
# End Project
