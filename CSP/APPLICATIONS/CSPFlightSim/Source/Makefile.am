# WARNING:
#   I have almost no prior experience with GNU autoconf/automake and this
#   is very much a work in progress.  The original Makefile for Linux can
#   be found in Source/Makefile.orig.  If you have trouble with building
#   CSPFlightSim with this configure script or autmake files, and know
#   how to fix them, please do so and commit the changes to CVS.  Thanks!
#
#   -OS


bin_PROGRAMS = CSPFlightSim

CSPFlightSim_SOURCES = \
	AAAController.cpp \
	AAAObject.cpp \
	AeroParam.cpp \
	AeroParamSymbol.cpp \
	AIAirplaneController.cpp \
	AirplaneController.cpp \
	AirplaneInput.cpp \
	AirplaneObject.cpp \
	AirplanePhysics.cpp \
	BaseController.cpp \
	base.cpp \
	BaseObject.cpp \
	BaseSymbol.cpp \
	CockpitDrawable.cpp \
	Config.cpp \
	ConsoleCommands.cpp \
	CSPFlightSim.cpp \
	DirVectorDrawable.cpp \
	GameScreen.cpp \
	GlobalCommands.cpp \
	global.cpp \
	Hud.cpp \
	HudElement.cpp \
	HudGeometricElement.cpp \
	HudTextElement.cpp \
	LogoScreen.cpp \
	logstream.cpp \
	main.cpp \
	MenuScreen.cpp \
	Message.cpp \
	MessageQueue.cpp \
	MissileController.cpp \
	MissileObject.cpp \
	mmgr.cpp \
	ObjectFactory.cpp \
	ObjectRangeInfo.cpp \
	ObjectSymbol.cpp \
	ObjectTypeSymbol.cpp \
	ParticleObject.cpp \
	Platform.cpp \
	ProjectileController.cpp \
	ProjectileObject.cpp \
	SimTime.cpp \
	sky.cpp \
	StaticObject.cpp \
	stdinc.cpp \
	SymbolTable.cpp \
	TankController.cpp \
	TankObject.cpp \
	TerrainObject.cpp \
	TerrainSymbol.cpp \
	Tools.cpp \
	trees.cpp \
	TypesMath.cpp \
	TypesMatrix3.cpp \
	TypesMatrix4.cpp \
	TypesQuaternion.cpp \
	TypesVector3.cpp \
	TypesVector4.cpp \
	VirtualBattlefield.cpp \
	VirtualBattlefieldScene.cpp

# You should not need to edit anything below here.
OSG_INCLUDES=$(OSG_PREFIX)/include
DEMETER_INCLUDES=$(DEMETER_PREFIX)
DEMETER_LIB_PREFIX=$(DEMETER_PREFIX)
DEMETER_LIBS=-L$(DEMETER_LIB_PREFIX) -ldemeter_csp


CSPFlightSim_LDADD = $(DEMETER_LIBS) 

AM_CPPFLAGS=-I@top_srcdir@/Include -I$(DEMETER_INCLUDES) -I$(OSG_INCLUDES) @SDL_FLAGS@ @GDAL_FLAGS@
CXXFLAGS=-g #-Wall

#CFLAGS+=-funroll-loops # -fomit-frame-pointer # not stable?
#CFLAGS+=-march=pentiumpro -mcpu=pentiumpro # not stable?
#CFLAGS+=-Wno-missing-braces -Wno-multichar -fsigned-char
