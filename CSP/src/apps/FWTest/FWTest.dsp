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
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\src\framework" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "DirectInput" /D "OGL" /D "D3D" /D "DSound" /D "DirectMath" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ws2_32.lib dsound.lib d3d8.lib dxguid.lib dinput8.lib winmm.lib d3dx8.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib opengl32.lib glaux.lib glu32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "FWTest - Win32 Release"
# Name "FWTest - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
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
# Begin Group "Framework"

# PROP Default_Filter ""
# Begin Group "Source"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\framework\audio\win\Audio_DX.cpp
# End Source File
# Begin Source File

SOURCE=..\..\framework\audio\AudioData.cpp
# End Source File
# Begin Source File

SOURCE=..\..\framework\graphics\win\Bitmap.cpp
# End Source File
# Begin Source File

SOURCE=..\..\framework\graphics\win\ChildWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\framework\graphics\win\D3D.cpp
# End Source File
# Begin Source File

SOURCE=..\..\framework\graphics\win\D3DCamera.cpp
# End Source File
# Begin Source File

SOURCE=..\..\framework\graphics\win\D3DMaterialObject.cpp
# End Source File
# Begin Source File

SOURCE=..\..\framework\graphics\win\D3DPolygonListObject.cpp
# End Source File
# Begin Source File

SOURCE=..\..\framework\graphics\win\D3DTexture2D.cpp
# End Source File
# Begin Source File

SOURCE=..\..\framework\input\win\DirectInput.cpp
# End Source File
# Begin Source File

SOURCE=..\..\framework\audio\FactoryAudio.cpp
# End Source File
# Begin Source File

SOURCE=..\..\framework\graphics\FactoryGraphics.cpp
# End Source File
# Begin Source File

SOURCE=..\..\framework\input\FactoryInput.cpp
# End Source File
# Begin Source File

SOURCE=..\..\framework\standard\FactoryMath.cpp
# End Source File
# Begin Source File

SOURCE=..\..\framework\net\FactoryNetwork.cpp
# End Source File
# Begin Source File

SOURCE=..\..\framework\platform\FactoryPlatform.cpp
# End Source File
# Begin Source File

SOURCE=..\..\framework\graphics\win\Font.cpp
# End Source File
# Begin Source File

SOURCE=..\..\framework\standard\FrameworkError.cpp
# End Source File
# Begin Source File

SOURCE=..\..\framework\graphics\gl_globals.cpp
# End Source File
# Begin Source File

SOURCE=..\..\framework\graphics\win\GLCamera.cpp
# End Source File
# Begin Source File

SOURCE=..\..\framework\graphics\win\GLMaterialObject.cpp
# End Source File
# Begin Source File

SOURCE=..\..\framework\graphics\win\GLPolygonListObject.cpp
# End Source File
# Begin Source File

SOURCE=..\..\framework\graphics\win\GLTexture2D.cpp
# End Source File
# Begin Source File

SOURCE=..\..\framework\graphics\win\MaterialFormat.cpp
# End Source File
# Begin Source File

SOURCE=..\..\framework\standard\mtxlib.cpp
# End Source File
# Begin Source File

SOURCE=..\..\framework\audio\NoAudio.cpp
# End Source File
# Begin Source File

SOURCE=..\..\framework\graphics\NoGraphics.cpp
# End Source File
# Begin Source File

SOURCE=..\..\framework\input\NoInput.cpp
# End Source File
# Begin Source File

SOURCE=..\..\framework\net\NoNetwork.cpp
# End Source File
# Begin Source File

SOURCE=..\..\framework\platform\NoPlatform.cpp
# End Source File
# Begin Source File

SOURCE=..\..\framework\graphics\win\openglgraphics.cpp
# End Source File
# Begin Source File

SOURCE=..\..\framework\standard\win\OpenGLMath.cpp
# End Source File
# Begin Source File

SOURCE=..\..\framework\platform\StandardPlatform.cpp
# End Source File
# Begin Source File

SOURCE=..\..\framework\graphics\win\Texture.cpp
# End Source File
# Begin Source File

SOURCE=..\..\framework\graphics\win\TextureFormat.cpp
# End Source File
# Begin Source File

SOURCE=..\..\framework\graphics\win\TextWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\framework\net\windows\WindowsNetwork.cpp
# End Source File
# Begin Source File

SOURCE=..\..\framework\platform\win\WindowsPlatform.cpp
# End Source File
# End Group
# Begin Group "Header"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\framework\audio\win\Audio_DX.h
# End Source File
# Begin Source File

SOURCE=..\..\framework\audio\AudioData.h
# End Source File
# Begin Source File

SOURCE=..\..\framework\graphics\win\Bitmap.h
# End Source File
# Begin Source File

SOURCE=..\..\framework\graphics\win\Camera.h
# End Source File
# Begin Source File

SOURCE=..\..\framework\graphics\win\ChildWindow.h
# End Source File
# Begin Source File

SOURCE=..\..\framework\graphics\win\D3D.h
# End Source File
# Begin Source File

SOURCE=..\..\framework\graphics\win\D3DCamera.h
# End Source File
# Begin Source File

SOURCE=..\..\framework\graphics\win\D3DMaterialObject.h
# End Source File
# Begin Source File

SOURCE=..\..\framework\graphics\win\D3DPolygonListObject.h
# End Source File
# Begin Source File

SOURCE=..\..\framework\graphics\win\D3DTexture2D.h
# End Source File
# Begin Source File

SOURCE=..\..\framework\input\win\DirectInput.h
# End Source File
# Begin Source File

SOURCE=..\..\framework\standard\win\DirectMath.h
# End Source File
# Begin Source File

SOURCE=..\..\framework\audio\FactoryAudio.h
# End Source File
# Begin Source File

SOURCE=..\..\framework\graphics\FactoryGraphics.h
# End Source File
# Begin Source File

SOURCE=..\..\framework\input\FactoryInput.h
# End Source File
# Begin Source File

SOURCE=..\..\framework\net\FactoryNetwork.h
# End Source File
# Begin Source File

SOURCE=..\..\framework\platform\FactoryPlatform.h
# End Source File
# Begin Source File

SOURCE=..\..\framework\graphics\win\Font.h
# End Source File
# Begin Source File

SOURCE=..\..\framework\graphics\gl_globals.h
# End Source File
# Begin Source File

SOURCE=..\..\framework\graphics\win\GLCamera.h
# End Source File
# Begin Source File

SOURCE=..\..\framework\graphics\glext.h
# End Source File
# Begin Source File

SOURCE=..\..\framework\graphics\win\GLMaterialObject.h
# End Source File
# Begin Source File

SOURCE=..\..\framework\graphics\win\GLPolygonListObject.h
# End Source File
# Begin Source File

SOURCE=..\..\framework\graphics\win\GLTexture2D.h
# End Source File
# Begin Source File

SOURCE=..\..\framework\graphics\GraphicsFormat.h
# End Source File
# Begin Source File

SOURCE=..\..\framework\graphics\win\GraphicsFormat.h
# End Source File
# Begin Source File

SOURCE=..\..\framework\graphics\win\GraphicsObject.h
# End Source File
# Begin Source File

SOURCE=..\..\framework\standard\License.h
# End Source File
# Begin Source File

SOURCE=..\..\framework\graphics\win\MaterialFormat.h
# End Source File
# Begin Source File

SOURCE=..\..\framework\audio\NoAudio.h
# End Source File
# Begin Source File

SOURCE=..\..\framework\graphics\NoGraphics.h
# End Source File
# Begin Source File

SOURCE=..\..\framework\input\NoInput.h
# End Source File
# Begin Source File

SOURCE=..\..\framework\net\NoNetwork.h
# End Source File
# Begin Source File

SOURCE=..\..\framework\platform\NoPlatform.h
# End Source File
# Begin Source File

SOURCE=..\..\framework\graphics\win\openglgraphics.h
# End Source File
# Begin Source File

SOURCE=..\..\framework\standard\win\OpenGLMath.h
# End Source File
# Begin Source File

SOURCE=..\..\framework\standard\SimpleMath.h
# End Source File
# Begin Source File

SOURCE=..\..\framework\app\StandardApp.h
# End Source File
# Begin Source File

SOURCE=..\..\framework\audio\StandardAudio.h
# End Source File
# Begin Source File

SOURCE=..\..\framework\file\StandardBuffer.h
# End Source File
# Begin Source File

SOURCE=..\..\framework\file\StandardFile.h
# End Source File
# Begin Source File

SOURCE=..\..\framework\standard\StandardFramework.h
# End Source File
# Begin Source File

SOURCE=..\..\framework\graphics\StandardGraphics.h
# End Source File
# Begin Source File

SOURCE=..\..\framework\input\StandardInput.h
# End Source File
# Begin Source File

SOURCE=..\..\framework\standard\StandardMath.h
# End Source File
# Begin Source File

SOURCE=..\..\framework\net\StandardNetwork.h
# End Source File
# Begin Source File

SOURCE=..\..\framework\platform\StandardPlatform.h
# End Source File
# Begin Source File

SOURCE=..\..\framework\graphics\win\Texture.h
# End Source File
# Begin Source File

SOURCE=..\..\framework\graphics\win\TextureFormat.h
# End Source File
# Begin Source File

SOURCE=..\..\framework\graphics\win\TextWindow.h
# End Source File
# Begin Source File

SOURCE=..\..\framework\graphics\win\tgainfo.h
# End Source File
# Begin Source File

SOURCE=..\..\framework\app\TypesApp.h
# End Source File
# Begin Source File

SOURCE=..\..\framework\audio\TypesAudio.h
# End Source File
# Begin Source File

SOURCE=..\..\framework\standard\TypesError.h
# End Source File
# Begin Source File

SOURCE=..\..\framework\standard\TypesFramework.h
# End Source File
# Begin Source File

SOURCE=..\..\framework\graphics\TypesGraphics.h
# End Source File
# Begin Source File

SOURCE=..\..\framework\input\TypesInput.h
# End Source File
# Begin Source File

SOURCE=..\..\framework\standard\TypesMath.h
# End Source File
# Begin Source File

SOURCE=..\..\framework\net\TypesNetwork.h
# End Source File
# Begin Source File

SOURCE=..\..\framework\platform\TypesPlatform.h
# End Source File
# Begin Source File

SOURCE=..\..\framework\net\windows\WindowsNetwork.h
# End Source File
# Begin Source File

SOURCE=..\..\framework\platform\win\WindowsPlatform.h
# End Source File
# End Group
# End Group
# Begin Source File

SOURCE=.\readme.txt
# End Source File
# End Target
# End Project
