# makefile for cspwf
SET(LIB_NAME cspwf)

ADD_DEFINITIONS(-DCSPWF_EXPORTS -D__CSPSIM_EXE__)

IF (WIN32)
	ADD_DEFINITIONS(-D_USRDLL -D_DLL)
ENDIF (WIN32)

SET (SOURCE_AND_HEADER_PATH ${csp_SOURCE_DIR}/csp/cspwf)

ADD_LIBRARY(${LIB_NAME} SHARED
			${csp_SOURCE_DIR}/csp/cspwf/Button.cpp
			${csp_SOURCE_DIR}/csp/cspwf/Button.h
			${csp_SOURCE_DIR}/csp/cspwf/Check.cpp
			${csp_SOURCE_DIR}/csp/cspwf/Check.h
			${csp_SOURCE_DIR}/csp/cspwf/CheckBox.cpp
			${csp_SOURCE_DIR}/csp/cspwf/CheckBox.h
			${csp_SOURCE_DIR}/csp/cspwf/Container.cpp
			${csp_SOURCE_DIR}/csp/cspwf/Container.h
			${csp_SOURCE_DIR}/csp/cspwf/Control.cpp
			${csp_SOURCE_DIR}/csp/cspwf/Control.h
			${csp_SOURCE_DIR}/csp/cspwf/ControlCallback.cpp
			${csp_SOURCE_DIR}/csp/cspwf/ControlCallback.h
			${csp_SOURCE_DIR}/csp/cspwf/ControlGeometryBuilder.cpp
			${csp_SOURCE_DIR}/csp/cspwf/ControlGeometryBuilder.h
			${csp_SOURCE_DIR}/csp/cspwf/ControlMoveEventHandler.cpp
			${csp_SOURCE_DIR}/csp/cspwf/ControlMoveEventHandler.h
			${csp_SOURCE_DIR}/csp/cspwf/Image.cpp
			${csp_SOURCE_DIR}/csp/cspwf/Image.h
			${csp_SOURCE_DIR}/csp/cspwf/Label.cpp
			${csp_SOURCE_DIR}/csp/cspwf/Label.h
			${csp_SOURCE_DIR}/csp/cspwf/ListBox.cpp
			${csp_SOURCE_DIR}/csp/cspwf/ListBox.h
			${csp_SOURCE_DIR}/csp/cspwf/ListBoxItem.cpp
			${csp_SOURCE_DIR}/csp/cspwf/ListBoxItem.h
			${csp_SOURCE_DIR}/csp/cspwf/Model.cpp
			${csp_SOURCE_DIR}/csp/cspwf/Model.h
			${csp_SOURCE_DIR}/csp/cspwf/MultiControlContainer.cpp
			${csp_SOURCE_DIR}/csp/cspwf/MultiControlContainer.h
			${csp_SOURCE_DIR}/csp/cspwf/ResourceLocator.cpp
			${csp_SOURCE_DIR}/csp/cspwf/ResourceLocator.h
			${csp_SOURCE_DIR}/csp/cspwf/ScrollBar.cpp
			${csp_SOURCE_DIR}/csp/cspwf/ScrollBar.h
			${csp_SOURCE_DIR}/csp/cspwf/Serialization.cpp
			${csp_SOURCE_DIR}/csp/cspwf/Serialization.h
			${csp_SOURCE_DIR}/csp/cspwf/Signal.cpp
			${csp_SOURCE_DIR}/csp/cspwf/Signal.h
			${csp_SOURCE_DIR}/csp/cspwf/SignalData.cpp
			${csp_SOURCE_DIR}/csp/cspwf/SignalData.h
			${csp_SOURCE_DIR}/csp/cspwf/SingleControlContainer.cpp
			${csp_SOURCE_DIR}/csp/cspwf/SingleControlContainer.h
			${csp_SOURCE_DIR}/csp/cspwf/Slot.cpp
			${csp_SOURCE_DIR}/csp/cspwf/Slot.h
			${csp_SOURCE_DIR}/csp/cspwf/StringResourceManager.cpp
			${csp_SOURCE_DIR}/csp/cspwf/StringResourceManager.h
			${csp_SOURCE_DIR}/csp/cspwf/Style.cpp
			${csp_SOURCE_DIR}/csp/cspwf/Style.h
			${csp_SOURCE_DIR}/csp/cspwf/StyleBuilder.cpp
			${csp_SOURCE_DIR}/csp/cspwf/StyleBuilder.h
			${csp_SOURCE_DIR}/csp/cspwf/Tab.cpp
			${csp_SOURCE_DIR}/csp/cspwf/Tab.h
			${csp_SOURCE_DIR}/csp/cspwf/TableControlContainer.cpp
			${csp_SOURCE_DIR}/csp/cspwf/TableControlContainer.h
			${csp_SOURCE_DIR}/csp/cspwf/Window.cpp
			${csp_SOURCE_DIR}/csp/cspwf/Window.h
			${csp_SOURCE_DIR}/csp/cspwf/WindowManager.cpp
			${csp_SOURCE_DIR}/csp/cspwf/WindowManager.h
			${csp_SOURCE_DIR}/csp/cspwf/WindowManagerEventHandler.cpp
			${csp_SOURCE_DIR}/csp/cspwf/WindowManagerEventHandler.h
			${csp_SOURCE_DIR}/csp/cspwf/WindowManagerViewer.cpp
			${csp_SOURCE_DIR}/csp/cspwf/WindowManagerViewer.h
			${csp_SOURCE_DIR}/csp/cspwf/WindowManagerViewerNode.cpp
			${csp_SOURCE_DIR}/csp/cspwf/WindowManagerViewerNode.h
			)
			
TARGET_LINK_LIBRARIES(${LIB_NAME} ${OSG_LIBS}
								  ${OPENGL_LIBS}
								  ${PYTHON_LIBRARY}
								  csplib		  
					 )

# We want all dlls and libs of this project in the same directory. Visual Studio puts project output files
# into individual directories per configuration (e.h. "debug" and "release"), so we have to copy the files
# from there into our own output dir in a separate step. CAUTION: The copy command uses environment 
# variables from Visual Studio. They should work for 2008 and 2010, but maybe not for future versions
IF(MSVC_IDE)
	ADD_CUSTOM_COMMAND(TARGET ${LIB_NAME}
					   POST_BUILD
					   COMMAND copy $(InputDir)$(ConfigurationName)\\*.* $(InputDir)..\\..\\bin
					   COMMENT "copy output files to common directory"
					  )
ENDIF(MSVC_IDE)

#ADD_DEPENDENCIES(cspwf csplib)
