# standard build flags

INCLUDE = -I.. -I../SimData/Include -I/usr/include/cc++2
CXXFLAGS = $(INCLUDE) -g -O2 -W -Wall -pedantic -Wno-long-long

SIMNET_LIB = -L$(TOPDIR)/SimNet -lSimNet
SIMDATA_LIB = -L$(TOPDIR)/SimData/SimData -lSimData

