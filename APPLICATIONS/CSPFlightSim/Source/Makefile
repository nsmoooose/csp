# Makefile for CSPFlightSim
# Clement Bourdarias <phneutre@icculus.org>
# last changed on Sunday, November 17th.

# Edit these lines to match your system configuration
CC=g++

OSG_PREFIX=/usr/local
GDAL_PREFIX=/usr/local
DEMETER_DIR=../../../THIRDPARTYLIBS/demeter

# nothing should be edited below this line
CSP_BINARY=CSPFlightSim

OSG_INCLUDES_PREFIX=$(OSG_PREFIX)/include
OSG_LIB_PREFIX=$(OSG_PREFIX)/lib
OSG_LIB=-losg -losgGLUT -losgParticle -losgText -losgUtil

GDAL_LIB_PREFIX=$(GDAL_PREFIX)/lib
GDAL_LIB=-lgdal.1.1

DEMETER_INCLUDES=$(DEMETER_DIR)
DEMETER_LIB=-ldemeter_csp

# if we want to statically link demeter
#DEMETER_OBJ=$(DEMETER_INCLUDES)/demeter_csp.a

LDFLAGS=$(shell sdl-config --libs) -lSDL_image -lSDL_console
LDFLAGS+=-L$(DEMETER_DIR) $(DEMETER_LIB)
LDFLAGS+=-L$(OSG_LIB_PREFIX) $(OSG_LIB)
LDFLAGS+=-L$(GDAL_LIB_PREFIX) $(GDAL_LIB)

CFLAGS=-Wall -g $(shell sdl-config --cflags) -I../Include/ -I$(DEMETER_INCLUDES) -I$(OSG_INCLUDES_PREFIX)
CFLAGS+=-w # get rid of warnings at the moment
#CFLAGS+=-funroll-loops # -fomit-frame-pointer # not stable?
#CFLAGS+=-march=pentiumpro -mcpu=pentiumpro # not stable?
#CFLAGS+=-Wno-missing-braces -Wno-multichar -fsigned-char

%.o: %.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

CSP_SOURCES = ./AAAController.cpp \
	./AAAObject.cpp \
	./AeroParam.cpp \
	./AeroParamSymbol.cpp \
	./AIAirplaneController.cpp \
	./AirplaneController.cpp \
	./AirplaneInput.cpp \
	./AirplaneObject.cpp \
	./AirplanePhysics.cpp \
	./BaseController.cpp \
	./base.cpp \
	./BaseObject.cpp \
	./BaseSymbol.cpp \
	./CockpitDrawable.cpp \
	./Config.cpp \
	./ConsoleCommands.cpp \
	./CSPFlightSim.cpp \
	./DirVectorDrawable.cpp \
	./GameScreen.cpp \
	./GlobalCommands.cpp \
	./global.cpp \
	./Hud.cpp \
	./HudElement.cpp \
	./HudGeometricElement.cpp \
	./HudTextElement.cpp \
	./LogoScreen.cpp \
	./logstream.cpp \
	./main.cpp \
	./MenuScreen.cpp \
	./Message.cpp \
	./MessageQueue.cpp \
	./MissileController.cpp \
	./MissileObject.cpp \
	./mmgr.cpp \
	./ObjectFactory.cpp \
	./ObjectRangeInfo.cpp \
	./ObjectSymbol.cpp \
	./ObjectTypeSymbol.cpp \
	./ParticleObject.cpp \
	./ProjectileController.cpp \
	./ProjectileObject.cpp \
	./SimTime.cpp \
	./sky.cpp \
	./StaticObject.cpp \
	./stdinc.cpp \
	./SymbolTable.cpp \
	./TankController.cpp \
	./TankObject.cpp \
	./TerrainObject.cpp \
	./TerrainSymbol.cpp \
	./Tools.cpp \
	./trees.cpp \
	./TypesMath.cpp \
	./TypesMatrix3.cpp \
	./TypesMatrix4.cpp \
	./TypesQuaternion.cpp \
	./TypesVector3.cpp \
	./TypesVector4.cpp \
	./VirtualBattlefield.cpp \
	./VirtualBattlefieldScene.cpp \

CSP_OBJECTS=$(CSP_SOURCES:.cpp=.o)

all: $(CSP_BINARY)

$(CSP_BINARY): $(CSP_OBJECTS)
	rm -rf $(CSP_BINARY)
	$(CC) -o $(CSP_BINARY) $(CSP_OBJECTS) $(LDFLAGS)

clean:
	rm -rf $(CSP_BINARY) $(CSP_OBJECTS)
	
install:
	# soon.
