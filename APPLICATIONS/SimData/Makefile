SUBDIRS = Source 

export GDEBUGF = -g -Wall
export GCFLAGS = -fPIC -O2 # -march=athlon-tbird
export GLDOPTS = -shared -lswigpy -ldl
export GSWOPTS = -c -c++ -python -noexcept
export CXX = g++
export SWIG = swig 

.PHONY: all clean clean-deps

all clean clean-deps: 
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $${dir} $@ || exit 1; \
	done

default: 
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $${dir} $@ || exit 1; \
	done

swig:
	rm Source/cSimData_wrap.cpp || true;
	make -C Source all

install:
	python setup.py make_install
