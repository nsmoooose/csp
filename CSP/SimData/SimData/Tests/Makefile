TOPDIR = ../..
INCLUDE = -I$(TOPDIR)/Include -I$(PYTHON_INCLUDE)

CFLAGS = $(GCFLAGS) $(INCLUDE) $(GDEBUGF)
SWCXXF = $(GCFLAGS) $(INCLUDE)
LDOPTS = $(GLDOPTS)
SWOPTS = $(GSWOPTS) $(INCLUDE)

TESTS_SOURCE = $(wildcard test_*.cpp)
TESTS = $(TESTS_SOURCE:%.cpp=%)

SOURCES = $(MODULES:%=%.cpp)
OBJECTS = $(MODULES:%=%.o) 

DEPDIR = .deps
MKDEP = $(CXX) -M $(CFLAGS)
SWDEP = $(SWIG) -M $(SWOPTS)
DEPFILES = $(addprefix $(DEPDIR)/,$(addsuffix .d, $(TESTS)))
DEPFILTER = | sed 's/\.o:/:/'
DEPS_MAGIC := $(shell mkdir $(DEPDIR) > /dev/null 2>&1 || :)

.PHONY: clean-deps clean all default

default:
	@echo "run make from top-level directory only"
	
clean: clean-deps
	rm -f $(TESTS)

clean-deps:
	rm -f $(DEPDIR)/*.d

ifeq ($(findstring clean,$(MAKECMDGOALS)),)
ifneq ($(strip $(DEPFILES)),)
-include $(DEPFILES)
endif
endif

FAILURES = 0

$(DEPDIR)/%.d : %
	@$(MKDEP) $<.cpp $(DEPFILTER) > $@
	
#@echo "Computing dependencies for $<..."

%: %.cpp 
	@echo -n "Building $@..."
	@for foo in a; do \
		$(CXX) -o $@ $(CFLAGS) $(@:=.cpp) -L$(TOPDIR)/SimData -lSimData -lpthread 2>.buildlog 1>/dev/null; \
		if [ $$? = "0" ]; then echo "ok"; else echo "failed! (see .buildlog for details)"; exit 1; fi; \
	done
	$

run:
	@export FAILED_TESTS=0; \
	for test in $(TESTS); do \
	  echo -n "Running  $$test..."; \
	  ./$$test 2>$$test.log 1>/dev/null; \
	  if [ $$? = "0" ]; then \
	    echo "ok"; \
	     rm $$test.log; \
	  else \
	     echo "failed!"; \
	     FAILED_TESTS=`expr $$FAILED_TESTS + 1`; \
	  fi; \
	done; \
	if [ $$FAILED_TESTS -gt 0 ]; then \
	  echo "********************************************"; \
	  echo "$$FAILED_TESTS TEST(S) FAILED!"; \
	  echo "********************************************"; \
	  exit 1; \
	fi
	$

all: $(TESTS) run

