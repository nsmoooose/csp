CC = gcc

Test : AirPlaneTest.o AirPlane.o mtxlib.o
	$(CC) -o Test -lm AirPlaneTest.o AirPlane.o mtxlib.o

AirPlaneTest.o : AirPlaneTest.cpp mtxlib.h
	$(CC) -c AirPlaneTest.cpp

AirPlane.o : AirPlane.cpp AirPlane.h mtxlib.h
	$(CC) -c AirPlane.cpp

mtxlib.o: mtxlib.cpp mtxlib.h
	$(CC) -c mtxlib.cpp
