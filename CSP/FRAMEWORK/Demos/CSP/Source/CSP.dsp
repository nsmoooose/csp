# Microsoft Developer Studio Project File - Name="CSP" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=CSP - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "CSP.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "CSP.mak" CFG="CSP - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CSP - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "CSP - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "CSP - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "D3D" /D "DirectAudio" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 wsock32.lib dsound.lib dxguid.lib dinput8.lib winmm.lib d3dx8.lib d3d8.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386 /out:"..\CSP.exe"

!ELSEIF  "$(CFG)" == "CSP - Win32 Debug"

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
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "D3D" /D "DirectAudio" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 wsock32.lib dsound.lib dxguid.lib dinput8.lib winmm.lib d3dx8.lib d3d8.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"..\CSP_d.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "CSP - Win32 Release"
# Name "CSP - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "FrameworkSRC"

# PROP Default_Filter ""
# Begin Group "DXsrc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\Source\Graphics\Dx8\WinDXCamera.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Graphics\Dx8\WinDXGraphics.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Graphics\Dx8\WinDXMaterial.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Graphics\DX8\WinDXPolygonBillboard.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Graphics\DX8\WinDXPolygonFlat.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Graphics\Dx8\WinDXPolygonMesh.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Graphics\Dx8\WinDXTexture.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Graphics\Dx8\WinDXViewport.cpp
# End Source File
# End Group
# Begin Group "OGLsrc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\Source\Graphics\OpenGL\Bitmap.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Graphics\OpenGL\GLCamera.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Graphics\OpenGL\GLFont.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Graphics\OpenGL\GLGlobals.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Graphics\OpenGL\GLGraphics.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Graphics\OpenGL\GLMaterial.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Graphics\OpenGL\GLPolygonMesh.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Graphics\OpenGL\GLTexture.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Graphics\OpenGL\GLViewport.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Graphics\OpenGL\Win\WinGLGraphics.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\Source\Audio\Win\AudioData.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Shared\GUI.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Shared\TypesButton.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Shared\TypesEdit.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Shared\TypesLabel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Shared\TypesLOD.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Math\TypesMatrix3.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Math\TypesMatrix4.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Shared\TypesScreen.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Math\TypesVector3.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Math\TypesVector4.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Shared\TypesWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\File\Win\WinBuff.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Audio\Win\WinDxAudio.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\File\Win\WinEndian.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Factory\Win\WinFactory.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\File\Win\WinFile.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Framework\Win\WinFramework.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Input\Win\WinInput.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Network\Win\WinNetwork.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Window\Win\WinWindow3D.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\Source\Window\Win\WinWindowText.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\App.cpp
# End Source File
# Begin Source File

SOURCE=.\Main.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "FrameworkH"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\Include\AudioData.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\Bitmap.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\Framework.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\GLGraphics.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\GUI.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\License.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\TypesApp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\TypesAudio.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\TypesBuffer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\TypesButton.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\TypesCamera.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\TypesEdit.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\TypesEndian.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\TypesError.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\TypesFactory.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\TypesFile.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\TypesFramework.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\TypesGraphics.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\TypesGUI.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\TypesInput.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\TypesLabel.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\TypesLOD.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\TypesMaterial.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\TypesMath.inl
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\TypesMatrix3.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\TypesMatrix4.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\TypesNetwork.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\TypesPolygonBillboard.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\TypesPolygonFlat.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\TypesPolygonMesh.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\TypesScreen.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\TypesTexture.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\TypesVector3.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\TypesVector3.inl
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\TypesVector4.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\TypesVector4.inl
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\TypesViewport.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\TypesWindow.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\TypesWindow3d.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\TypesWindowText.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\WinBuff.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\WinDXAudio.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\WinDXCamera.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\WinDXGraphics.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\WinDXMaterial.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\WinDXPolygonBillboard.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\WinDXPolygonFlat.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\WinDXPolygonMesh.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\WinDXTexture.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\WinDXViewport.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\WinEndian.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\WinFile.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\WinFramework.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\WinGLGraphics.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\WinInput.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\WinNetwork.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\WinWindow3D.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\WinWindowText.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\App.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
