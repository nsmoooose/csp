TOPDIR = ../..

BIN = $(TOPDIR)/Bin

SUBDIRS =

LIBS = 

INCLUDE = -I$(TOPDIR)/include -I/usr/include/python2.2 -I$(TOPDIR)/../osgNV/include

BASEFL = $(GDEBUGF) $(GCFLAGS) $(INCLUDE) -D_REENTRANT -DDEBUG
CFLAGS = $(filter-out -I/usr/local/include, $(BASEFL)) -g
SWCXXF = $(GCFLAGS) $(INCLUDE)
LDOPTS = $(GLDOPTS) -losg -ldl -losgNV -losgNVCg -losgNVParse #-lpthread
#-L/usr/local/lib -lccgnu2 -ldl -pthread -losgParticle  -losgText  -losgUtil  -losg  -lSDL_image -L/usr/local/lib -Wl,-rpath,/usr/local/lib -lSDL -lpthread 
SWOPTS = $(GSWOPTS) $(INCLUDE)
AR = ar
LIBDIR = $(TOPDIR)/lib

SOURCES = \
	ChunkLod.cpp \
	ChunkLodDrawable.cpp \
	ChunkLodLoader.cpp \
	RayStrip.cpp \
	TextureQuadTree.cpp \
	TriangleIntersection.cpp \
	Version.cpp

OBJECTS = $(SOURCES:%.cpp=%.o) 

DEPDIR = .deps
MKDEP = $(CXX) -M $(CFLAGS)
SWDEP = $(SWIG) -M $(SWOPTS)
DEPFILES = $(addprefix $(DEPDIR)/,$(addsuffix .d, $(SOURCES))) $(DEPDIR)/cCSP.i.swigdep
DEPFILTER = 
DEPS_MAGIC := $(shell mkdir $(DEPDIR) > /dev/null 2>&1 || :)

.PHONY: clean-deps clean all default

default:
	@echo "runing make from top-level directory"
	make -C $(TOPDIR)
	
clean-objects:
	rm -f $(SOURCES:%.cpp=%.o)
	rm -f lib*.a
	rm -f $(BIN)/_cCSP.so $(BIN)/cCSP.py cCSP_wrap.*
	rm -f $(DEPDIR)/*.d

clean-dependencies:
	@echo $(RM) -r $(RMFLAGS) $(DEPDIR)

clean-deps: clean-dependencies

clean: clean-deps clean-objects

ifeq ($(findstring clean,$(MAKECMDGOALS)),)
ifneq ($(strip $(DEPFILES)),)
-include $(DEPFILES)
endif
endif

#clean clean-deps:
#	for dir in $(SUBDIRS); do \
#		$(MAKE) -C $${dir} $@; \
#	done
	
build-subdirs:
#	@for dir in $(SUBDIRS); do \
#		$(MAKE) -C $${dir} all; \
#	done

$(DEPDIR)/%.d : %
	@echo "Computing dependencies for $<..."
	@$(MKDEP) $< $(DEPFILTER) | sed -e 's!^[a-zA-Z0-9_]*.o:!$(<:%.cpp=%.o):!' > $@
	
$(DEPDIR)/%.swigdep : %
	@echo "Computing dependencies for $<..."
	@$(SWDEP) $(DEPFILTER) -o $(<:.i=_wrap.cpp) $< > $@

%.o: %.cpp 
	$(CXX) -c $(CFLAGS) -o $@ $(@:.o=.cpp)

#Makefile: Makefile.in
#	cd $(TOPDIR) && ./configure

#all: Makefile build-subdirs _cCSP.so #CSPapp
all: libosgChunkLod.so libosgChunkLod.a #CSPapp

libosgChunkLod.so: $(OBJECTS)
	$(CXX) $(LDOPTS) -o $@ $^
	#cp $@ $(TOPDIR)/../../APPLICATIONS/CSPSim/Bin

libosgChunkLod.a: $(OBJECTS)
	$(AR) rc $@ $^
	cp $@ $(LIBDIR)

genpath: Path.o
	$(CXX) $(LDOPTS) -o $@ $^ -L. -losgChunkLod -ljpeg

skin: Skin.o
	$(CXX) $(LDOPTS) -o $@ $^ -L. -losgChunkLod -ljpeg

#ChunkLod.o: ChunkLod.cpp raystrip.cpp

