PYTHONLIB = /usr/lib/python2.2/site-packages
PYTHONINC = /usr/include/python2.2

all: csptile

csptile: tile.cpp
	g++ -O2 -g $^ -o $@ -I$(PYTHONINC) -L$(PYTHONLIB)/SimData -lSimData -lz


