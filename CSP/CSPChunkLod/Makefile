SUBDIRS = src/osgChunkLod
#SUBDIRS = src/Demos src/osgChunkLod

export GDEBUGF = -g -Wall
export GCFLAGS = -fPIC -O2# -DSWIG_GLOBAL #-march=athon-tbird
export GLDOPTS = -shared
export GSWOPTS = -c -c++ -python -noexcept
export CXX = g++
export SWIG = swig 

.PHONY: all clean clean-deps swig default

default: all

all clean clean-deps: Makefile
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $${dir} $@ || exit 1; \
	done

#Makefile: Makefile.in configure.in acinclude.m4
#	aclocal ; autoconf
#	./configure



