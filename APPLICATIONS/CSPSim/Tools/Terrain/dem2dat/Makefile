PYTHONLIB = /usr/lib/python2.2/site-packages
PYTHONINC = /usr/include/python2.2

all: dem2dat

dem2dat: dem2dat.cpp dem2dat.h
	g++ -O2 -g dem2dat.cpp -o dem2dat -I$(PYTHONINC) -L$(PYTHONLIB)/SimData -lSimData -lz

python: dem2dat_wrap.o
	g++ -shared -lswigpy -ldl -o _dem2dat.so dem2dat_wrap.o -L$(PYTHONLIB)/SimData -lSimData -lz

dem2dat_wrap.o: dem2dat_wrap.cxx
	g++ -c -fPIC -O2 -g dem2dat_wrap.cxx -I$(PYTHONINC)

dem2dat_wrap.cxx: dem2dat.i dem2dat.h
	swig -c -c++ -python -noexcept -I$(PYTHONINC) dem2dat.i

