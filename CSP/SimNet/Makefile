INCLUDE = -I.. -I../SimData/Include -I/usr/include/cc++2
CXXFLAGS = $(INCLUDE) -g -pg -DNOTHREADS -O2 -DSIMNET_NDEBUG

BINDIR = .bin
DEPDIR = .dep
DOCDIR = .dox

MODULES = \
	test \
	NetRandom \
	NetworkInterface \
	NetworkNode \
	PeerInfo \
	RecordCodec \
	TestMessage

SOURCES = $(MODULES:%=%.cpp)
OBJECTS = $(MODULES:%=$(BINDIR)/%.o)

MKDEP = $(CXX) -M $(CXXFLAGS)
SWDEP = $(SWIG) -M $(SWOPTS)
DEPFILES = $(addprefix $(DEPDIR)/,$(addsuffix .d, $(SOURCES)))
DEPFILTER =
DEPS_MAGIC := $(shell mkdir $(DEPDIR) > /dev/null 2>&1 || :)
BIN_MAGIC := $(shell mkdir $(BINDIR) > /dev/null 2>&1 || :)
DOC_MAGIC := $(shell mkdir $(DOCDIR) > /dev/null 2>&1 || :)

.PHONY: clean-deps clean all default update_version

default: all

clean-objects:
	$(RM) -r $(RMFLAGS) $(BINDIR)

clean-dependencies:
	$(RM) -r $(RMFLAGS) $(DEPDIR)

clean-docs clean-dox clean-doc:
	$(RM) -r $(RMFLAGS) $(DOCDIR)

clean-deps: clean-dependencies

clean: clean-deps clean-objects clean-docs

ifeq ($(findstring clean,$(MAKECMDGOALS)),)
ifneq ($(strip $(DEPFILES)),)
-include $(DEPFILES)
endif
endif


$(DEPDIR)/%.d : %
	@echo "Computing dependencies for $<..."
	@echo $(MKDEP) $< $(DEPFILTER) > $@
	@$(MKDEP) $< $(DEPFILTER) > $@

$(DEPDIR)/%.swigdep : %
	@echo "Computing dependencies for $<..."
	@$(SWDEP) $(DEPFILTER) -o $(<:.i=_wrap.cpp) $< > $@

test: $(OBJECTS)
	g++ -o $@ $^ -L../SimData/SimData -lSimData -lpthread -ldl -lccgnu2 -pg

$(BINDIR)/%.o: %.cpp
	$(CXX) -o $@ -c $(CXXFLAGS) $^

%.cpp %.h: %.net
	../SimData/Tools/TaggedRecordCompiler/trc.py $<

libSimNet.a: $(OBJECTS)
	ar cru $@ $^
	ranlib $@

all: test libSimNet.a

dox docs doc:
	@echo "Generating documentation..."
	doxygen

