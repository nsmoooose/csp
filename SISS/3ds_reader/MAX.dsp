# Microsoft Developer Studio Project File - Name="MAX" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=MAX - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MAX.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MAX.mak" CFG="MAX - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MAX - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "MAX - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MAX - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /I "I:\OPENGL\3DS\MAX" /I "I:\OPENGL\LOD" /I "I:\CSP\FRAMEWORK\INCLUDE" /I "I:\CSP\FRAMEWORK\SOURCE\MATH" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 opengl32.lib glu32.lib glaux.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "MAX - Win32 Debug"

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
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "I:\OPENGL\3DS\MAX" /I "I:\OPENGL\LOD" /I "I:\CSP\FRAMEWORK" /I "I:\CSP\FRAMEWORK\INCLUDE" /I "I:\CSP\FRAMEWORK\GRAPHICS\OPENGL" /I "I:\CSP\FRAMEWORK\SOURCE" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "OGL" /FD /GZ /c
# SUBTRACT CPP /X /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 opengl32.lib glu32.lib glaux.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "MAX - Win32 Release"
# Name "MAX - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\CSP\Framework\Source\Graphics\OpenGL\GLTexture.cpp
# End Source File
# Begin Source File

SOURCE=.\maxldr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\lod\progmesh.cpp
# End Source File
# Begin Source File

SOURCE=..\..\lod\rabdata.cpp
# End Source File
# Begin Source File

SOURCE=.\test.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\CSP\Framework\Source\Math\TypesMatrix3.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\CSP\Framework\Source\Math\TypesMatrix4.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\CSP\Framework\Source\Math\TypesVector3.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\CSP\Framework\Source\Math\TypesVector4.cpp
# End Source File
# Begin Source File

SOURCE=..\..\lod\vector.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\lod\font.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CSP\Framework\Include\Framework.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CSP\Framework\Include\GLTexture.h
# End Source File
# Begin Source File

SOURCE=..\..\lod\list.h
# End Source File
# Begin Source File

SOURCE=.\maxldr.h
# End Source File
# Begin Source File

SOURCE=..\..\lod\progmesh.h
# End Source File
# Begin Source File

SOURCE=..\..\lod\rabdata.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CSP\Framework\Include\TypesMatrix3.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CSP\Framework\Include\TypesMatrix4.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CSP\Framework\Include\TypesVector3.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CSP\Framework\Include\TypesVector4.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CSP\Framework\Include\TypesVector4.inl
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
