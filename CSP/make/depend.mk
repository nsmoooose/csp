# Dependency tracking for c/c++/swig files.
#
# Notes:
#   Include after default rule is defined, and make 'clean'
#   depend on 'clean-deps'.
#
# Inputs:
#   SOURCES: list of source files to track (no extensions)
#   MESSAGES: list of messages descriptions to track (no extensions)
#   INTERFACES: list of swig interfaces to track (no extensions)


DEPDIR = .dep

MKDEP = $(CXX) -M $(CXXFLAGS)
SWDEP = $(SWIG) -M $(SWOPTS)
DEPSOURCES = $(SOURCES:%=%.cpp.d) $(MESSAGES:%=%.cpp.d) $(INTERFACES:%=%.swigd)
DEPFILES := $(addprefix $(DEPDIR)/,$(DEPSOURCES))
DEPSELF = | sed -n 's/^\(.*\):/\1 $(subst /,\/,$@):/;p'
DEPFILTER :=
DEPS_MAGIC := $(shell mkdir $(DEPDIR) > /dev/null 2>&1 || :)

.PHONY: clean-deps clean-dependencies

CLEAN:=$(CLEAN) clean-deps

clean-deps clean-dependencies:
	$(RM) -r $(RMFLAGS) $(DEPDIR)

ifeq ($(findstring clean,$(MAKECMDGOALS)),)
ifneq ($(strip $(DEPFILES)),)
-include $(DEPFILES)
endif
endif


$(DEPDIR)/%.d : %
	@echo "Computing dependencies for $<..."
	@echo $(MKDEP) $< $(DEPFILTER) $(DEPSELF) > $@
	@$(MKDEP) $< $(DEPFILTER) $(DEPSELF) > $@

$(DEPDIR)/%.swigd : %
	@echo "Computing dependencies for $<..."
	@$(SWDEP) -o $(<:.i=_wrap.cpp) $< $(DEPFILTER) $(DEPSELF) > $@
