CSP_PATH = ../../../..
SIMDATA_INC  = $(CSP_PATH)/SimData/Include
SIMDATA_LIB  = $(CSP_PATH)/SimData/SimData

all: csptile

csptile: tile.cpp
	g++ -W -Wall -O2 -g $^ -o $@ -I$(SIMDATA_INC) -L$(SIMDATA_LIB) -lSimData -lz -lpthread

clean:
	$(RM) csptile
