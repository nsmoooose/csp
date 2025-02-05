export PYTHONPATH := $(CURDIR)

CPU_COUNT=$(shell nproc)

all:
	cd csp;scons -j$(CPU_COUNT) all

clean:
	cd csp;scons -c

tests:
	cd csp;scons -j$(CPU_COUNT) runtests

loc:
	@echo "Number of lines of C++ code:"
	@find -type d -name ".bin" -prune -o -type f -name "*.cpp" -or -name "*.h" -print|xargs cat|wc -l
	@echo "Number of lines of python code:"
	@find -type d -name ".bin" -prune -o -type f -name "*.py" -print|xargs cat|wc -l

format:
	clang-format -i $(shell find -type d -name ".bin" -prune -o -type f -name "*.cpp" -or -name "*.h" -print)

scan: clean
	scan-build make all

.PHONY: all clean tests format scan
