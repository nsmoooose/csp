PYTHONLIB = /usr/lib/python2.3/site-packages
PYTHONINC = /usr/include/python2.3

CXX = g++
CXXFLAGS = -O2 -g -W -Wall
SWIG = swig

all: dem2dat

dem2dat: dem2dat.cpp dem2dat.h
	$(CXX) $(CXXFLAGS) dem2dat.cpp -o dem2dat -I$(PYTHONINC) -L$(PYTHONLIB)/SimData -lSimData -lz

python: dem2dat_wrap.o
	$(CXX) -shared -lswigpy -ldl -o _dem2dat.so dem2dat_wrap.o -L$(PYTHONLIB)/SimData -lSimData -lz

dem2dat_wrap.o: dem2dat_wrap.cxx
	$(CXX) -c -fPIC $(CXXFLAGS) dem2dat_wrap.cxx -I$(PYTHONINC)

dem2dat_wrap.cxx: dem2dat.i dem2dat.h
	$(SWIG) -c -c++ -python -noexcept -I$(PYTHONINC) dem2dat.i

