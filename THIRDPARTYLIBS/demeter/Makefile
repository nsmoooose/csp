# Makefile for Demeter
# Clement Bourdarias <phneutre@icculus.org>
# last changed on Sunday, November 17th.

# Edit these lines to match your system configuration
CC=g++ #-3.0
AR=ar
RANLIB=ranlib

# nothing should be edited below this line
CODE_BINARY=demeter_csp.a
SHARED_LIB=libdemeter_csp.so

CFLAGS=-Wall $(shell sdl-config --cflags)
CFLAGS+=-D_USE_GDAL_
CFLAGS+=-w

%.o: %.cpp
	$(CC) -fPIC -c -o $@ $< $(CFLAGS)

CODE_SOURCES = ./DemeterDrawable.cpp \
	./Terrain.cpp \
	./TerrainTextureFactory.cpp

CODE_OBJECTS=$(CODE_SOURCES:.cpp=.o)

all: $(CODE_BINARY)

$(CODE_BINARY): $(CODE_OBJECTS)
	rm -rf $(CODE_BINARY)
	$(AR) rc $(CODE_BINARY) $(CODE_OBJECTS)
	rm -rf $(SHARED_LIB)
	$(CC) -shared $(CODE_OBJECTS) -o $(SHARED_LIB)

clean:
	rm -rf $(CODE_BINARY) $(CODE_OBJECTS) $(SHARED_LIB)
	
install:
	# soon.
