# Microsoft Developer Studio Generated NMAKE File, Based on GroundCombat.dsp
!IF "$(CFG)" == ""
CFG=GroundCombat - Win32 Debug
!MESSAGE No configuration specified. Defaulting to GroundCombat - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "GroundCombat - Win32 Release" && "$(CFG)" != "GroundCombat - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "GroundCombat.mak" CFG="GroundCombat - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "GroundCombat - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "GroundCombat - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "GroundCombat - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\GroundCombat.exe"


CLEAN :
	-@erase "$(INTDIR)\Globals.obj"
	-@erase "$(INTDIR)\GroundCombat.obj"
	-@erase "$(INTDIR)\GroundCombat.pch"
	-@erase "$(INTDIR)\MainInterface.obj"
	-@erase "$(INTDIR)\MainManager.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\TerrainManager.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\GroundCombat.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\GroundCombat.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\GroundCombat.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\GroundCombat.pdb" /machine:I386 /out:"$(OUTDIR)\GroundCombat.exe" 
LINK32_OBJS= \
	"$(INTDIR)\Globals.obj" \
	"$(INTDIR)\GroundCombat.obj" \
	"$(INTDIR)\MainInterface.obj" \
	"$(INTDIR)\MainManager.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"..\..\..\auran\lib\AIExtension.Dev.lib" \
	"..\..\..\auran\lib\AIFoundation.Dev.lib" \
	"..\..\..\auran\lib\CollisionSystem.Dev.lib" \
	"..\..\..\auran\lib\InputExtension.Dev.lib" \
	"..\..\..\auran\lib\InputSystem.Dev.lib" \
	"..\..\..\auran\lib\InterfaceResources.Dev.lib" \
	"..\..\..\auran\lib\InterfaceSystem.Dev.lib" \
	"..\..\..\auran\lib\JIVELib.Dev.lib" \
	"..\..\..\auran\lib\Kernel.Dev.lib" \
	"..\..\..\auran\lib\KernelResource.Dev.lib" \
	"..\..\..\auran\lib\KernelResources.Dev.lib" \
	"..\..\..\auran\lib\KernelStandard.Dev.lib" \
	"..\..\..\auran\lib\Lexer.Dev.lib" \
	"..\..\..\auran\lib\MovementSystem.Dev.lib" \
	"..\..\..\auran\lib\NetworkSystem.Dev.lib" \
	"..\..\..\auran\lib\Render2DCore.Dev.lib" \
	"..\..\..\auran\lib\Render3DCore.Dev.lib" \
	"..\..\..\auran\lib\Render3DExtension.Dev.lib" \
	"..\..\..\auran\lib\Render3DFoundation.Dev.lib" \
	"..\..\..\auran\lib\RenderSystem.Dev.lib" \
	"..\..\..\auran\lib\SoundCore.Dev.lib" \
	"..\..\..\auran\lib\WaveClient.Dev.lib" \
	"..\..\..\auran\lib\WaveServer.Dev.lib" \
	"..\..\..\auran\lib\WaveSystem.Dev.lib" \
	"..\..\..\auran\lib\Window3D.Dev.lib" \
	"$(INTDIR)\TerrainManager.obj"

"$(OUTDIR)\GroundCombat.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "GroundCombat - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : ".\GroundCombat.exe" "$(OUTDIR)\GroundCombat.pch" "$(OUTDIR)\GroundCombat.bsc"


CLEAN :
	-@erase "$(INTDIR)\Globals.obj"
	-@erase "$(INTDIR)\Globals.sbr"
	-@erase "$(INTDIR)\GroundCombat.obj"
	-@erase "$(INTDIR)\GroundCombat.pch"
	-@erase "$(INTDIR)\GroundCombat.sbr"
	-@erase "$(INTDIR)\MainInterface.obj"
	-@erase "$(INTDIR)\MainInterface.sbr"
	-@erase "$(INTDIR)\MainManager.obj"
	-@erase "$(INTDIR)\MainManager.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\TerrainManager.obj"
	-@erase "$(INTDIR)\TerrainManager.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\GroundCombat.bsc"
	-@erase "$(OUTDIR)\GroundCombat.pdb"
	-@erase ".\GroundCombat.exe"
	-@erase ".\GroundCombat.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /Gr /MDd /W4 /Gm /GR /GX /ZI /Od /I "D:\Development\Auran\Inc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\GroundCombat.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\Globals.sbr" \
	"$(INTDIR)\GroundCombat.sbr" \
	"$(INTDIR)\MainInterface.sbr" \
	"$(INTDIR)\MainManager.sbr" \
	"$(INTDIR)\StdAfx.sbr" \
	"$(INTDIR)\TerrainManager.sbr"

"$(OUTDIR)\GroundCombat.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\GroundCombat.pdb" /debug /machine:I386 /out:"GroundCombat.exe" /pdbtype:sept /libpath:"D:\Development\Auran\Lib" 
LINK32_OBJS= \
	"$(INTDIR)\Globals.obj" \
	"$(INTDIR)\GroundCombat.obj" \
	"$(INTDIR)\MainInterface.obj" \
	"$(INTDIR)\MainManager.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"..\..\..\auran\lib\AIExtension.Dev.lib" \
	"..\..\..\auran\lib\AIFoundation.Dev.lib" \
	"..\..\..\auran\lib\CollisionSystem.Dev.lib" \
	"..\..\..\auran\lib\InputExtension.Dev.lib" \
	"..\..\..\auran\lib\InputSystem.Dev.lib" \
	"..\..\..\auran\lib\InterfaceResources.Dev.lib" \
	"..\..\..\auran\lib\InterfaceSystem.Dev.lib" \
	"..\..\..\auran\lib\JIVELib.Dev.lib" \
	"..\..\..\auran\lib\Kernel.Dev.lib" \
	"..\..\..\auran\lib\KernelResource.Dev.lib" \
	"..\..\..\auran\lib\KernelResources.Dev.lib" \
	"..\..\..\auran\lib\KernelStandard.Dev.lib" \
	"..\..\..\auran\lib\Lexer.Dev.lib" \
	"..\..\..\auran\lib\MovementSystem.Dev.lib" \
	"..\..\..\auran\lib\NetworkSystem.Dev.lib" \
	"..\..\..\auran\lib\Render2DCore.Dev.lib" \
	"..\..\..\auran\lib\Render3DCore.Dev.lib" \
	"..\..\..\auran\lib\Render3DExtension.Dev.lib" \
	"..\..\..\auran\lib\Render3DFoundation.Dev.lib" \
	"..\..\..\auran\lib\RenderSystem.Dev.lib" \
	"..\..\..\auran\lib\SoundCore.Dev.lib" \
	"..\..\..\auran\lib\WaveClient.Dev.lib" \
	"..\..\..\auran\lib\WaveServer.Dev.lib" \
	"..\..\..\auran\lib\WaveSystem.Dev.lib" \
	"..\..\..\auran\lib\Window3D.Dev.lib" \
	"$(INTDIR)\TerrainManager.obj"

".\GroundCombat.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("GroundCombat.dep")
!INCLUDE "GroundCombat.dep"
!ELSE 
!MESSAGE Warning: cannot find "GroundCombat.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "GroundCombat - Win32 Release" || "$(CFG)" == "GroundCombat - Win32 Debug"
SOURCE=.\Globals.cpp

!IF  "$(CFG)" == "GroundCombat - Win32 Release"


"$(INTDIR)\Globals.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\GroundCombat.pch"


!ELSEIF  "$(CFG)" == "GroundCombat - Win32 Debug"


"$(INTDIR)\Globals.obj"	"$(INTDIR)\Globals.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\GroundCombat.cpp

!IF  "$(CFG)" == "GroundCombat - Win32 Release"


"$(INTDIR)\GroundCombat.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\GroundCombat.pch"


!ELSEIF  "$(CFG)" == "GroundCombat - Win32 Debug"


"$(INTDIR)\GroundCombat.obj"	"$(INTDIR)\GroundCombat.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\MainInterface.cpp

!IF  "$(CFG)" == "GroundCombat - Win32 Release"


"$(INTDIR)\MainInterface.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\GroundCombat.pch"


!ELSEIF  "$(CFG)" == "GroundCombat - Win32 Debug"


"$(INTDIR)\MainInterface.obj"	"$(INTDIR)\MainInterface.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\MainManager.cpp

!IF  "$(CFG)" == "GroundCombat - Win32 Release"


"$(INTDIR)\MainManager.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\GroundCombat.pch"


!ELSEIF  "$(CFG)" == "GroundCombat - Win32 Debug"


"$(INTDIR)\MainManager.obj"	"$(INTDIR)\MainManager.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "GroundCombat - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\GroundCombat.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\GroundCombat.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "GroundCombat - Win32 Debug"

CPP_SWITCHES=/nologo /Gr /MDd /W4 /Gm /GR /GX /ZI /Od /I "D:\Development\Auran\Inc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\GroundCombat.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\GroundCombat.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\TerrainManager.cpp

!IF  "$(CFG)" == "GroundCombat - Win32 Release"


"$(INTDIR)\TerrainManager.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\GroundCombat.pch"


!ELSEIF  "$(CFG)" == "GroundCombat - Win32 Debug"


"$(INTDIR)\TerrainManager.obj"	"$(INTDIR)\TerrainManager.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 


!ENDIF 

