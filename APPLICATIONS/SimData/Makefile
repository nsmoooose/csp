SUBDIRS = Source 

export PYTHON_INCLUDE := -I$(shell python setup.py python_include_path)

export GDEBUGF = -g -Wall
export GCFLAGS = -fPIC -O2 $(PYTHON_INCLUDE)# -march=athlon-tbird
export GLDOPTS = -shared -lswigpy -ldl
export GSWOPTS = -c -c++ -python -noexcept
export CXX = g++
export SWIG = swig 

.PHONY: all clean clean-deps

all clean clean-deps: 
	@python setup.py check_version
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $${dir} $@ || exit 1; \
	done

default: 
	@python setup.py version
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $${dir} $@ || exit 1; \
	done

install:
	@python setup.py version
	@python setup.py make_install
