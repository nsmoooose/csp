# WARNING:
#   I have almost no prior experience with GNU autoconf/automake and this
#   is very much a work in progress.  The original Makefile for Linux can
#   be found in Source/Makefile.orig.  If you have trouble with building
#   CSPFlightSim with this configure script or autmake files, and know
#   how to fix them, please do so and commit the changes to CVS.  Thanks!
#
#   -OS

bin_PROGRAMS = CSPSim


# 	AAAController.cpp \
# 	AAAObject.cpp \
# 	
# 	AIAirplaneController.cpp \
# 	AirplaneController.cpp \
# 	AirplaneInput.cpp \
# 	AirplaneObject.cpp \
# 	AirplanePhysics.cpp \
# 	
# 	AeroParamSymbol.cpp \
#
#	BaseController.cpp \
# 	BaseObject.cpp \
# 	BaseSymbol.cpp \
# 	
#	CockpitDrawable.cpp \
#
# 	Hud.cpp \
# 	HudElement.cpp \
# 	HudGeometricElement.cpp \
# 	HudTextElement.cpp \
# 
# 	MissileController.cpp \
# 	MissileObject.cpp \
# 	
# 	ObjectSymbol.cpp \
# 	ObjectTypeSymbol.cpp \
# 	ParticleObject.cpp \
# 	
# 	ProjectileController.cpp \
# 	ProjectileObject.cpp \
# 	
# 	SymbolTable.cpp \
# 	TankController.cpp \
# 	TankObject.cpp \
# 	
# 	TypesMath.cpp \
# 	TypesMatrix3.cpp \
# 	TypesMatrix4.cpp \
# 	TypesQuaternion.cpp \
# 	TypesVector3.cpp \
# 	TypesVector4.cpp \
#
# 	MessageQueue.cpp \
#
	
CSPSim_SOURCES = \
	AeroDynamics.cpp \
	AircraftObject.cpp \
	base.cpp \
	BaseScreen.cpp \
	Config.cpp \
	ConsoleCommands.cpp \
	CSPSim.cpp \
	DynamicObject.cpp \
	EventMapIndex.cpp \
	EventMapping.cpp \
	F16Model.cpp \
	GameScreen.cpp \
	HID.cpp \
	InputInterface.cpp \
	LogoScreen.cpp \
	logstream.cpp \
	main.cpp \
	MenuScreen.cpp \
 	Message.cpp \
	mmgr.cpp \
	ObjectModel.cpp \
	ObjectRangeInfo.cpp \
	Platform.cpp \
	SimObject.cpp \
	SimpleConfig.cpp \
	sky.cpp \
	StaticObject.cpp \
	TankObject.cpp \
	TerrainObject.cpp \
	Tools.cpp \
	trees.cpp \
	VirtualBattlefield.cpp 

	# VirtualBattlefieldScene.cpp
	#SimTime.cpp 
	#StaticObject.cpp
	#DirVectorDrawable.cpp
	#GlobalCommands.cpp 
	#CSPFlightSim.cpp
	#ObjectFactory.cpp 
	#AeroParam.cpp 
	#TerrainSymbol.cpp

# You should not need to edit anything below here.
OSG_INCLUDES=$(OSG_PREFIX)/include
DEMETER_INCLUDES=$(DEMETER_PREFIX)
DEMETER_LIB_PREFIX=$(DEMETER_PREFIX)
DEMETER_LIBS=-L$(DEMETER_LIB_PREFIX) -ldemeter_csp

SIMDATA_INCLUDES=$(SIMDATA_PREFIX)/Include
SIMDATA_LIB_PREFIX=$(SIMDATA_PREFIX)/Source/.libs
#SIMDATA_LIBS=-L$(SIMDATA_LIB_PREFIX) -lSimData
SIMDATA_LIBS=-L. -lSimData

CSPSim_LDADD = $(DEMETER_LIBS) $(SIMDATA_LIBS)

AM_CPPFLAGS=-I@top_srcdir@/Include -I$(SIMDATA_INCLUDES) -I$(DEMETER_INCLUDES) -I$(OSG_INCLUDES) @SDL_FLAGS@ @GDAL_FLAGS@
CXXFLAGS=-g #-Wall

#CFLAGS+=-funroll-loops # -fomit-frame-pointer # not stable?
#CFLAGS+=-march=pentiumpro -mcpu=pentiumpro # not stable?
#CFLAGS+=-Wno-missing-braces -Wno-multichar -fsigned-char

SWIG = swig
SWOPTS = -c -c++ -python $(AM_CPPFLAGS) -I/usr/local/include

all-local: _CSP.so

_CSP.so: CSP_wrap.o
	$(CXX) -shared -lswigpy -ldl -o $@ $(DEMETER_LIBS) $(SIMDATA_LIBS) $(LIBS) $(filter-out main.o, $(CSPSim_SOURCES:.cpp=.o)) _cSimData.so $^

CSP_wrap.o: CSP_wrap.cxx
	$(CXX) -c CSP_wrap.cxx -I../Include $(CXXFLAGS) $(AM_CPPFLAGS) -I/usr/local/include/python2.2 

CSP_wrap.cxx: CSP.i CSPSim
	$(SWIG) $(SWOPTS) CSP.i

