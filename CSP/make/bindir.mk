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

$(BINDIR)/%.o: %.cpp
	$(CXX) -o $@ -c $(CXXFLAGS) $<

