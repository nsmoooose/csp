CC      = g++
TARGET  = fwdemo

CFLAGS  = -Wall -pedantic
SOURCE  =
DEFINES = -DLINUX -DOGL -g3
LIBS    = -L/usr/X11R6/lib -lGL -lGLU -lXxf86vm

CSPPATH = ../../..
INCLUDES= -I$(CSPPATH)/src/framework
OBJECTS = main.o app.o\
	$(CSPPATH)/src/framework/audio/FactoryAudio.o\
	$(CSPPATH)/src/framework/audio/NoAudio.o\
	$(CSPPATH)/src/framework/graphics/FactoryGraphics.o\
	$(CSPPATH)/src/framework/graphics/NoGraphics.o\
	$(CSPPATH)/src/framework/graphics/linux/OpenGLGraphics.o\
	$(CSPPATH)/src/framework/input/FactoryInput.o\
	$(CSPPATH)/src/framework/input/NoInput.o\
	$(CSPPATH)/src/framework/net/FactoryNetwork.o\
	$(CSPPATH)/src/framework/net/NoNetwork.o\
	$(CSPPATH)/src/framework/platform/FactoryPlatform.o\
	$(CSPPATH)/src/framework/platform/linux/LinuxPlatform.o\
	$(CSPPATH)/src/framework/platform/NoPlatform.o\
	$(CSPPATH)/src/framework/platform/StandardPlatform.o\
	$(CSPPATH)/src/framework/standard/FrameworkError.o\
	$(CSPPATH)/src/framework/standard/FactoryMath.o

#
#
#

DO_CC   = $(CC) $(CFLAGS) $(HP_DEBUG) $(DEFINES) $(INCLUDES) -o $@ -c $< 
.cpp.o:
	$(DO_CC)


all: $(OBJECTS)
	$(CC) -o $(TARGET) $(OBJECTS) $(INCLUDES) $(LIBS)


clean:
	rm -f $(TARGET) $(OBJECTS)

tidy:
	make clean
	rm -f *~
