# rules to run doxygen and clean up the documentation directory
#
# Doxyfile should exist in the current directory.  Documentation
# will be generated in ./.dox/

DOCDIR = .dox

DOC_MAGIC := $(shell mkdir $(DOCDIR) > /dev/null 2>&1 || :)

.PHONY: clean-docs clean-dox clean-doc dox docs doc

CLEAN:=$(CLEAN) clean-docs

clean-docs clean-dox clean-doc:
	$(RM) -r $(RMFLAGS) $(DOCDIR)

dox docs doc:
	@echo "Generating documentation..."
	doxygen
