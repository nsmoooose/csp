CPPFLAGS = @CPPFLAGS@ 
## -DOGL

##INCLUDES = -I@top_srcdir@/Include
noinst_LTLIBRARIES = libLinuxFactory.la
libLinuxFactory_la_SOURCES = LinuxFactory.cpp
libLinuxFactory_la_LIBADD = @top_srcdir@/Source/Audio/Linux/libLinuxAudio.la \
           @top_srcdir@/Source/Framework/Linux/libLinuxFramework.la  \
           @top_srcdir@/Source/Input/SDL/libSDLInput.la \
           @top_srcdir@/Source/Network/Linux/libLinuxNetwork.la \
           @top_srcdir@/Source/Window/SDL/libSDLWindow.la \
           @top_srcdir@/Source/File/Linux/libLinuxFile.la \
           @top_srcdir@/Source/Graphics/OpenGL/libOpenGLCommon.la \
           @top_srcdir@/Source/Graphics/OpenGL/SDL/libOpenGLSDL.la \
           @top_srcdir@/Source/Shared/libLinuxGUI.la
