# Standard Makefile includes
#
# Notes:
#   This file must be included after defining the source
#   files with SOURCES, MESSAGES, INTERFACES, etc.
#
# Example:
#
#   TOPDIR = ...
#   SOURCES = ...
#   INTERFACES = ...
#
#   include $(TOPDIR)/make/standard.mk
#
#   custom targets:
#           rules...
#

.PHONY: all
default: all

include $(TOPDIR)/make/init.mk
include $(TOPDIR)/make/flags.mk
include $(TOPDIR)/make/depend.mk
include $(TOPDIR)/make/bindir.mk
include $(TOPDIR)/make/doxygen.mk
include $(TOPDIR)/make/clean.mk
include $(TOPDIR)/make/rules.mk
