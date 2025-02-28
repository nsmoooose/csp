export PYTHONPATH := $(CURDIR)

CPU_COUNT=$(shell nproc)

all:
	cd csp;scons -j$(CPU_COUNT) all debug=1

clean:
	cd csp;scons -c

tests:
	cd csp;scons -j$(CPU_COUNT) runtests

run:
	cd csp/bin; ./sim.py

run-server:
	cd csp/bin; ./sim-server.py

run-client1:
	cd csp/bin; ./sim.py --ini sim-client1.ini --logcat BATTLEFIELD,MESSAGE,PEER,HANDSHAKE,PACKET,TIMING

run-client2:
	cd csp/bin; ./sim.py --ini sim-client2.ini

# A target for generating a definition of all compile commands.
# This is used if you have Emacs as code editor together with
# lsp-mode and clangd for quick code navigation.
compile_commands.json:
	bear -- $(MAKE) clean all

loc:
	@echo "Number of lines of C++ code:"
	@find -type d -name ".bin" -prune -o -type f -name "*.cpp" -or -name "*.h" -print|xargs cat|wc -l
	@echo "Number of lines of python code:"
	@find -type d -name ".bin" -prune -o -type f -name "*.py" -print|xargs cat|wc -l

format:
	clang-format -i $(shell find -type d -name ".bin" -prune -o -type f -name "*.cpp" -or -name "*.h" -print)

scan: clean
	scan-build make all

.PHONY: all clean tests run run-server run-client1 run-client2 loc format scan
