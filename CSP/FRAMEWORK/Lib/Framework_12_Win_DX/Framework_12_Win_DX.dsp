# Microsoft Developer Studio Project File - Name="Framework_12_Win_DX" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Framework_12_Win_DX - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Framework_12_Win_DX.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Framework_12_Win_DX.mak" CFG="Framework_12_Win_DX - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Framework_12_Win_DX - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Framework_12_Win_DX - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Framework_12_Win_DX - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /G5 /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "DirectAudio" /D "D3D" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\Framework_12_Win_DX.lib"

!ELSEIF  "$(CFG)" == "Framework_12_Win_DX - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ  /c
# ADD CPP /nologo /G5 /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "DirectAudio" /D "D3D" /FD /GZ  /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\Framework_12_Win_DX_d.lib"

!ENDIF 

# Begin Target

# Name "Framework_12_Win_DX - Win32 Release"
# Name "Framework_12_Win_DX - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\Source\Audio\Win\AudioData.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\Shared\GUI.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\Shared\TypesButton.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\Shared\TypesEdit.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\Shared\TypesLabel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\Shared\TypesLOD.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\Math\TypesMatrix3.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\Math\TypesMatrix4.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\Shared\TypesScreen.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\Math\TypesVector3.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\Math\TypesVector4.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\Shared\TypesWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\File\Win\WinBuff.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\Audio\Win\WinDxAudio.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\Graphics\Dx8\WinDXCamera.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\Graphics\Dx8\WinDXGraphics.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\Graphics\Dx8\WinDXMaterial.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\Graphics\Dx8\WinDXPolygonBillboard.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\Graphics\Dx8\WinDXPolygonFlat.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\Graphics\Dx8\WinDXPolygonMesh.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\Graphics\Dx8\WinDXTexture.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\Graphics\Dx8\WinDXViewport.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\File\Win\WinEndian.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\Factory\Win\WinFactory.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\File\Win\WinFile.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\Framework\Win\WinFramework.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\Input\Win\WinInput.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\Network\Win\WinNetwork.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\Window\Win\WinWindow3D.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Source\Window\Win\WinWindowText.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# End Target
# End Project
