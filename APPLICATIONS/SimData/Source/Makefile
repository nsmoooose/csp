TOPDIR = ..
INCLUDE = -I$(TOPDIR)/Include -I$(PYTHON_INCLUDE)

CFLAGS = $(GCFLAGS) $(INCLUDE) $(GDEBUGF)
SWCXXF = $(GCFLAGS) $(INCLUDE)
LDOPTS = $(GLDOPTS)
SWOPTS = $(GSWOPTS) $(INCLUDE)

#SUBDIRS = Types

MODULES = \
	BaseType \
	DataArchive \
	DataManager \
	Date \
	Enum \
	Exception \
	External \
	FileUtility \
	GeoPos \
	HashUtility \
	InterfaceRegistry \
	Interpolate \
	Link \
	List \
	LogStream \
	Math \
	Matrix3 \
	Noise \
	Object \
	Pack \
	Path \
	Quaternion \
	Random \
	Real \
	TypeAdapter \
	Vector3 \
	Version

SOURCES = $(MODULES:%=%.cpp)
OBJECTS = $(MODULES:%=%.o) 

DEPDIR = .deps
MKDEP = $(CXX) -M $(CFLAGS)
SWDEP = $(SWIG) -M $(SWOPTS)
DEPFILES = $(addprefix $(DEPDIR)/,$(addsuffix .d, $(SOURCES))) $(DEPDIR)/cSimData.i.swigdep
DEPFILTER =
DEPS_MAGIC := $(shell mkdir $(DEPDIR) > /dev/null 2>&1 || :)

.PHONY: clean-deps clean all default

default:
	@echo "run make from top-level directory only"
	
clean-objects:
	rm -f $(MODULES:%=_%.so)
	rm -f $(MODULES:%=%.o)
	rm -f lib*.a
	rm -f _cSimData.so cSimData_wrap.*
	rm -f $(MODULES:%=%_wrap.o)
	rm -f $(DEPDIR)/*.d
	rm -f $(TOPDIR)/SimData/_cSimData.so
	rm -f $(TOPDIR)/SimData/cSimData.py

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
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $${dir} all; \
	done

$(DEPDIR)/%.d : %
	@echo "Computing dependencies for $<..."
	@$(MKDEP) $< $(DEPFILTER) > $@
	
$(DEPDIR)/%.swigdep : %
	@echo "Computing dependencies for $<..."
	@$(SWDEP) $(DEPFILTER) -o $(<:.i=_wrap.cpp) $< > $@

libSimData.a: $(OBJECTS)
	ar cru $@ $^
	ranlib $@
	cp $@ $(TOPDIR)/SimData

_cSimData.so: $(OBJECTS) cSimData_wrap.o
	$(CXX) -Wl,-z,lazyload $(LDOPTS) -o$@ $^
	cp $@ $(TOPDIR)/SimData

cSimData_wrap.cpp: cSimData.i
	$(SWIG) $(SWOPTS) -o $@ $< 
	cp cSimData.py $(TOPDIR)/SimData

cSimData_wrap.o: cSimData_wrap.cpp
	$(CXX) -c $(SWCXXF) -I$(PYTHON_INCLUDE) $(@:.o=.cpp)

%.o: %.cpp 
	$(CXX) -c $(CFLAGS) $(@:.o=.cpp)

all: _cSimData.so libSimData.a
#all: build-subdirs _cSimData.so libSimData.a

