SUBDIRS = Audio File Math Input Network Graphics Window Framework Shared Factory

## INCLUDES = @INCLUDES@

AUTOMAKE_OPTIONS                 = foreign 
lib_LTLIBRARIES                  = libFramework.la

FRAMEWORK_LIBS = Audio/Linux/libLinuxAudio.la \
           Input/SDL/libSDLInput.la \
           Network/Linux/libLinuxNetwork.la \
           Window/SDL/libSDLWindow.la \
           File/Linux/libLinuxFile.la \
           Math/libFrameworkMath.la \
           Graphics/OpenGL/libOpenGLCommon.la \
           Graphics/OpenGL/SDL/libOpenGLSDL.la \
           Shared/libLinuxGUI.la \
           Factory/Linux/libLinuxFactory.la \
           Framework/Linux/libLinuxFramework.la	   

libFramework_la_SOURCES      = dummy.c
libFramework_la_LDFLAGS      = -no-undefined -version-info 1:0:0
libFramework_la_LIBADD       =   $(FRAMEWORK_LIBS)

libFramework_la_DEPENDENCIES = $(FRAMEWORK_LIBS)



