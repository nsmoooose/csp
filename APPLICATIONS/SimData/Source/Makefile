TOPDIR = ..
INCLUDE = -I$(TOPDIR)/Include

CFLAGS = $(GCFLAGS) $(INCLUDE) $(GDEBUGF)
SWCXXF = $(GCFLAGS) $(INCLUDE)
LDOPTS = $(GLDOPTS)
SWOPTS = $(GSWOPTS) $(INCLUDE)

#SUBDIRS = Types

MODULES = \
	BaseType \
	DataArchive \
	Date \
	Enum \
	Exception \
	External \
	GeoPos \
	HashUtility \
	InterfaceRegistry \
	Interpolate \
	List \
	LogStream \
	Math \
	Matrix3 \
	Object \
	Pack \
	Path \
	Quaternion \
	Random \
	Spread \
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

_cSimData.so: $(OBJECTS) cSimData_wrap.o
	$(CXX) -Wl,-z,lazyload $(LDOPTS) -o$@ $^

cSimData_wrap.cpp: cSimData.i
	$(SWIG) $(SWOPTS) -o $@ $< 

cSimData_wrap.o: cSimData_wrap.cpp
	$(CXX) -I/usr/local/include/python2.2 -DSWIG_GLOBAL -c $(SWCXXF) $(@:.o=.cpp)

%.o: %.cpp 
	$(CXX) -c $(CFLAGS) $(@:.o=.cpp)

all: _cSimData.so libSimData.a
#all: build-subdirs _cSimData.so libSimData.a

