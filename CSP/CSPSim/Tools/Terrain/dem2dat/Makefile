CSP_PATH = ../../../..
SIMDATA_INC  = $(CSP_PATH)/SimData/Include
SIMDATA_LIB  = $(CSP_PATH)/SimData/SimData

CXX = g++
CXXFLAGS = -O2 -g -W -Wall -I$(SIMDATA_INC)
SWIG = swig

all: dem2dat

dem2dat: dem2dat.cpp dem2dat.h
	$(CXX) $(CXXFLAGS) dem2dat.cpp -o dem2dat -L$(SIMDATA_LIB) -lSimData -lz -lpthread

python: dem2dat_wrap.o
	$(CXX) -shared -lswigpy -ldl -o _dem2dat.so dem2dat_wrap.o -L$(SIMDATA_LIB) -lSimData -lz

dem2dat_wrap.o: dem2dat_wrap.cxx
	$(CXX) -c -fPIC $(CXXFLAGS) dem2dat_wrap.cxx

dem2dat_wrap.cxx: dem2dat.i dem2dat.h
	$(SWIG) -c -c++ -python -noexcept dem2dat.i

clean:
	$(RM) dem2dat_wrap.o dem2dat dem2dat_wrap.cxx dem2dat.py
