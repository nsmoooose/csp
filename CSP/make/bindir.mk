# create object files in a separate directory

BINDIR = .bin
BIN_MAGIC := $(shell mkdir $(BINDIR) > /dev/null 2>&1 || :)

# dependecy rules need to refer to the objects in the bindir
DEPFILTER:=$(DEPFILTER) | sed -n 's/^\(.*:\)/$(BINDIR)\/\1/;p'

.PHONY: clean-objects

CLEAN:=$(CLEAN) clean-objects

clean-objects:
	$(RM) -r $(RMFLAGS) $(BINDIR)

OBJECTS = $(SOURCES:%=$(BINDIR)/%.o)
MSGOBJS = $(MESSAGES:%=$(BINDIR)/%.o)

.PRECIOUS: $(BINDIR)/%_wrap.os
.PRECIOUS: %_wrap.cpp

$(BINDIR)/%.o: %.cpp
	$(CXX) -o $@ -c $(CXXFLAGS) $<

$(BINDIR)/%_wrap.os: %_wrap.cpp
	$(CXX) -o $@ -c $(subst -W ,,$(subst -pedantic,,$(subst -Wall,,$(CXXFLAGS)))) $<

_%.so: $(BINDIR)/%_wrap.os $(OBJECTS)
	$(CXX) -shared -Wl,-z,lazyload $(LDOPTS) -L$(PYTHONLIB) -o$@ $^


