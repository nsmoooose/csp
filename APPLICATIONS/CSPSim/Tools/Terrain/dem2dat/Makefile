all: dem2dat

dem2dat: dem2dat.cpp dem2dat.h
	g++ -O2 -g dem2dat.cpp -o dem2dat -I/usr/include/python2.2 -lSimData

python: dem2dat_wrap.o
	g++ -shared -lswigpy -ldl -o _dem2dat.so dem2dat_wrap.o  -lSimData

dem2dat_wrap.o: dem2dat_wrap.cxx
	g++ -c -fPIC -O2 -g dem2dat_wrap.cxx -I/usr/include/python2.2

dem2dat_wrap.cxx: dem2dat.i dem2dat.h
	swig -c -c++ -python -noexcept -I/usr/include/python2.2 dem2dat.i
