# Microsoft Developer Studio Project File - Name="SimData" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=SimData - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SimData.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SimData.mak" CFG="SimData - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SimData - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "SimData - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SimData - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SIMDATA_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "../Include" /I "C:/Python22/include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SIMDATA_EXPORTS" /FD /c
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
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"Release/_cSimData.dll" /libpath:"C:/Python22/libs"

!ELSEIF  "$(CFG)" == "SimData - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SIMDATA_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "../Include" /I "C:/Python22/include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SIMDATA_EXPORTS" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"Debug/SimDatad.bsc"
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"Debug/_cSimDatad.dll" /pdbtype:sept /libpath:"C:/Python22/libs"

!ENDIF 

# Begin Target

# Name "SimData - Win32 Release"
# Name "SimData - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\Source\BaseType.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\cSimData_wrap.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\DataArchive.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Date.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Enum.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Exception.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\External.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\HashUtility.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\InterfaceRegistry.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Interpolate.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\List.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\main.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Math.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Matrix3.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Object.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Pack.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Path.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Quaternion.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Random.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\SimDataEntryPoint.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Spread.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\TypeAdapter.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Vector3.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\Include\SimData\BaseType.h
# End Source File
# Begin Source File

SOURCE=..\Include\SimData\DataArchive.h
# End Source File
# Begin Source File

SOURCE=..\Include\SimData\Date.h
# End Source File
# Begin Source File

SOURCE=..\Include\SimData\Enum.h
# End Source File
# Begin Source File

SOURCE=..\Include\SimData\Exception.h
# End Source File
# Begin Source File

SOURCE=..\Include\SimData\Export.h
# End Source File
# Begin Source File

SOURCE=..\Include\SimData\External.h
# End Source File
# Begin Source File

SOURCE=..\Include\SimData\GlibCsp.h
# End Source File
# Begin Source File

SOURCE=..\Include\SimData\hash_map.h
# End Source File
# Begin Source File

SOURCE=..\Include\SimData\HashUtility.h
# End Source File
# Begin Source File

SOURCE=..\Include\SimData\Integer.h
# End Source File
# Begin Source File

SOURCE=..\Include\SimData\InterfaceRegistry.h
# End Source File
# Begin Source File

SOURCE=..\Include\SimData\InterfaceRegistry_wrap.h
# End Source File
# Begin Source File

SOURCE=..\Include\SimData\Interpolate.h
# End Source File
# Begin Source File

SOURCE=..\Include\SimData\List.h
# End Source File
# Begin Source File

SOURCE=..\Include\SimData\Math.h
# End Source File
# Begin Source File

SOURCE=..\Include\SimData\Matrix3.h
# End Source File
# Begin Source File

SOURCE="..\Include\SimData\ns-simdata.h"
# End Source File
# Begin Source File

SOURCE=..\Include\SimData\Object.h
# End Source File
# Begin Source File

SOURCE=..\Include\SimData\ObjectInterface.h
# End Source File
# Begin Source File

SOURCE=..\Include\SimData\Pack.h
# End Source File
# Begin Source File

SOURCE=..\Include\SimData\Path.h
# End Source File
# Begin Source File

SOURCE=..\Include\SimData\Quaternion.h
# End Source File
# Begin Source File

SOURCE=..\Include\SimData\Random.h
# End Source File
# Begin Source File

SOURCE=..\Include\SimData\Real.h
# End Source File
# Begin Source File

SOURCE=..\Include\SimData\Spread.h
# End Source File
# Begin Source File

SOURCE=..\Include\SimData\String.h
# End Source File
# Begin Source File

SOURCE=..\Include\SimData\TypeAdapter.h
# End Source File
# Begin Source File

SOURCE=..\Include\SimData\Types.h
# End Source File
# Begin Source File

SOURCE=..\Include\SimData\Vector3.h
# End Source File
# Begin Source File

SOURCE=..\Include\SimData\Vector3.inl
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=..\Source\cSimData.i

!IF  "$(CFG)" == "SimData - Win32 Release"

# Begin Custom Build
InputPath=..\Source\cSimData.i
InputName=cSimData

"..\Source\$(InputName)_wrap.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	C:\progra~1\SWIG-1.3.17\swig -c++ -python -noexcept -DWIN32 -I..\Include -o ..\Source\$(InputName)_wrap.cpp $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "SimData - Win32 Debug"

# Begin Custom Build
InputPath=..\Source\cSimData.i
InputName=cSimData

"..\Source\$(InputName)_wrap.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	C:\progra~1\SWIG-1.3.17\swig -c++ -python -noexcept -DWIN32 -I..\Include -o ..\Source\$(InputName)_wrap.cpp $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# End Target
# End Project
