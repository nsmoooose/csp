# Microsoft Developer Studio Project File - Name="Framework" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Framework - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Framework.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Framework.mak" CFG="Framework - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Framework - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Framework - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Framework", BAAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Framework - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "Framework - Win32 Debug"

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
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "./include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "OGL" /D "D3D" /D "DirectAudio" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 opengl32.lib glu32.lib glaux.lib ws2_32.lib d3dx8.lib d3d8.lib dxguid.lib dinput8.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib dsound.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Framework - Win32 Release"
# Name "Framework - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Audio"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Source\Audio\Win\AudioData.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Audio\Win\WinDxAudio.cpp
# End Source File
# End Group
# Begin Group "OpenGL"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Source\Graphics\OpenGL\Bitmap.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Graphics\OpenGL\GLCamera.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Graphics\OpenGL\GLFont.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Graphics\OpenGL\GLGlobals.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Graphics\OpenGL\GLGraphics.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Graphics\OpenGL\GLMaterial.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Graphics\OpenGL\GLPolygonMesh.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Graphics\OpenGL\GLTexture.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Graphics\OpenGL\GLViewport.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Graphics\OpenGL\Win\WinGLGraphics.cpp
# End Source File
# End Group
# Begin Group "DirectX8"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Source\Graphics\Dx8\WinDXCamera.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Graphics\Dx8\WinDXGraphics.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Graphics\Dx8\WinDXMaterial.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Graphics\Dx8\WinDXPolygonMesh.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Graphics\Dx8\WinDXTexture.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Graphics\Dx8\WinDXViewport.cpp
# End Source File
# End Group
# Begin Group "File"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Source\File\WinBuff.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\File\WinEndian.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\File\WinFile.cpp
# End Source File
# End Group
# Begin Group "Input"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Source\Input\Win\WinInput.cpp
# End Source File
# End Group
# Begin Group "Math"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Source\Math\TypesMatrix3.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Math\TypesMatrix4.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Math\TypesVector3.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Math\TypesVector4.cpp
# End Source File
# End Group
# Begin Group "Network"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Source\Network\Win\WinNetwork.cpp
# End Source File
# End Group
# Begin Group "Shared"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Source\Shared\GUI.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Shared\WinFactory.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Shared\WinFramework.cpp
# End Source File
# End Group
# Begin Group "Window"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Source\Window\Win\WinWindow3D.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Window\Win\WinWindowText.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\GLPolygonFlat.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Include\AudioData.h
# End Source File
# Begin Source File

SOURCE=.\Include\Bitmap.h
# End Source File
# Begin Source File

SOURCE=.\Include\Framework.h
# End Source File
# Begin Source File

SOURCE=.\Include\GLCamera.h
# End Source File
# Begin Source File

SOURCE=.\Include\GLExt.h
# End Source File
# Begin Source File

SOURCE=.\Include\GLFont.h
# End Source File
# Begin Source File

SOURCE=.\Include\GLGlobals.h
# End Source File
# Begin Source File

SOURCE=.\Include\GLGraphics.h
# End Source File
# Begin Source File

SOURCE=.\Include\GLLinuxGraphics.h
# End Source File
# Begin Source File

SOURCE=.\Include\GLMaterial.h
# End Source File
# Begin Source File

SOURCE=.\GLPolygonFlat.h
# End Source File
# Begin Source File

SOURCE=.\Include\GLPolygonMesh.h
# End Source File
# Begin Source File

SOURCE=.\Include\GLTexture.h
# End Source File
# Begin Source File

SOURCE=.\Include\GLViewport.h
# End Source File
# Begin Source File

SOURCE=.\Include\GUI.h
# End Source File
# Begin Source File

SOURCE=.\Include\License.h
# End Source File
# Begin Source File

SOURCE=.\Include\StandardMath.h
# End Source File
# Begin Source File

SOURCE=.\Include\TextureFormat.h
# End Source File
# Begin Source File

SOURCE=.\Include\TgaInfo.h
# End Source File
# Begin Source File

SOURCE=.\Include\TypesApp.h
# End Source File
# Begin Source File

SOURCE=.\Include\TypesAudio.h
# End Source File
# Begin Source File

SOURCE=.\Include\TypesBuffer.h
# End Source File
# Begin Source File

SOURCE=.\Include\TypesCamera.h
# End Source File
# Begin Source File

SOURCE=.\Include\TypesEndian.h
# End Source File
# Begin Source File

SOURCE=.\Include\TypesError.h
# End Source File
# Begin Source File

SOURCE=.\Include\TypesFactory.h
# End Source File
# Begin Source File

SOURCE=.\Include\TypesFile.h
# End Source File
# Begin Source File

SOURCE=.\Include\TypesFramework.h
# End Source File
# Begin Source File

SOURCE=.\Include\TypesGraphics.h
# End Source File
# Begin Source File

SOURCE=.\Include\TypesGUI.h
# End Source File
# Begin Source File

SOURCE=.\Include\TypesInput.h
# End Source File
# Begin Source File

SOURCE=.\Include\TypesMaterial.h
# End Source File
# Begin Source File

SOURCE=.\Include\TypesMath.h
# End Source File
# Begin Source File

SOURCE=.\Include\TypesMatrix3.h
# End Source File
# Begin Source File

SOURCE=.\Include\TypesMatrix4.h
# End Source File
# Begin Source File

SOURCE=.\Include\TypesNetwork.h
# End Source File
# Begin Source File

SOURCE=.\Include\TypesPolygonMesh.h
# End Source File
# Begin Source File

SOURCE=.\Include\TypesTexture.h
# End Source File
# Begin Source File

SOURCE=.\Include\TypesVector3.h
# End Source File
# Begin Source File

SOURCE=.\Include\TypesVector3.inl
# End Source File
# Begin Source File

SOURCE=.\Include\TypesVector4.h
# End Source File
# Begin Source File

SOURCE=.\Include\TypesVector4.inl
# End Source File
# Begin Source File

SOURCE=.\Include\TypesViewport.h
# End Source File
# Begin Source File

SOURCE=.\Include\TypesWindow3d.h
# End Source File
# Begin Source File

SOURCE=.\Include\TypesWindowText.h
# End Source File
# Begin Source File

SOURCE=.\Include\WinBuff.h
# End Source File
# Begin Source File

SOURCE=.\Include\WinDXAudio.h
# End Source File
# Begin Source File

SOURCE=.\Include\WinDXCamera.h
# End Source File
# Begin Source File

SOURCE=.\Include\WinDXGraphics.h
# End Source File
# Begin Source File

SOURCE=.\Include\WinDXLowLevel.h
# End Source File
# Begin Source File

SOURCE=.\Include\WinDXMaterial.h
# End Source File
# Begin Source File

SOURCE=.\Include\WinDXPolygonMesh.h
# End Source File
# Begin Source File

SOURCE=.\Include\WinDXTexture.h
# End Source File
# Begin Source File

SOURCE=.\Include\WinDXViewport.h
# End Source File
# Begin Source File

SOURCE=.\Include\WinEndian.h
# End Source File
# Begin Source File

SOURCE=.\Include\WinFile.h
# End Source File
# Begin Source File

SOURCE=.\Include\WinGLGraphics.h
# End Source File
# Begin Source File

SOURCE=.\Include\WinInput.h
# End Source File
# Begin Source File

SOURCE=.\Include\WinNetwork.h
# End Source File
# Begin Source File

SOURCE=.\Include\WinWindow3D.h
# End Source File
# Begin Source File

SOURCE=.\Include\WinWindowText.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "App"

# PROP Default_Filter ""
# Begin Group "Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Demos\DynamicCube\App.cpp
# End Source File
# Begin Source File

SOURCE=.\Demos\DynamicCube\Main.cpp
# End Source File
# End Group
# Begin Group "Include"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Demos\DynamicCube\App.h
# End Source File
# End Group
# End Group
# End Target
# End Project
