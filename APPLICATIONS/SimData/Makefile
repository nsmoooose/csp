SUBDIRS = Source 

export GDEBUGF = -g -Wall
export GCFLAGS = -fPIC -DUSE_NAMESPACE_SIMDATA -O2
export GLDOPTS = -shared -lswigpy -ldl
export GSWOPTS = -c -c++ -python -DUSE_NAMESPACE_SIMDATA -noexcept
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
