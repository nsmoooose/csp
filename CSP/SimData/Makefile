SUBDIRS = Source 

export PYTHON_INCLUDE := $(shell python setup.py python_include_path)
export GSWOPTS = $(shell python setup.py swigopts)
export GLDOPTS = $(shell python setup.py ldopts)

export GDEBUGF = -g -W -Wall -pedantic -Wmissing-prototypes -Wconversion -Wshadow #-DSIMDATA_NOLOADCHECK
export GCFLAGS = -fPIC -O2
export CXX = g++
export SWIG = swig

.PHONY: all clean clean-deps

all clean clean-deps: 
	@python setup.py check_version
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $${dir} $@ || exit 1; \
	done

doc:
	$(MAKE) -C Doc

default: 
	@python setup.py check_version
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $${dir} $@ || exit 1; \
	done

install:
	@python setup.py check_version
	@python setup.py make_install --verbose

tests:
	@$(MAKE) --no-print-directory -C SimData/Tests all

tests-clean:
	@$(MAKE) --no-print-directory -C SimData/Tests clean
