export PYTHONPATH := $(CURDIR)

CPU_COUNT=$(shell nproc)

all:
	cd csp;scons -j$(CPU_COUNT) all

clean:
	cd csp;scons -c

tests:
	cd csp;scons -j$(CPU_COUNT) runtests

.PHONY: all clean tests
