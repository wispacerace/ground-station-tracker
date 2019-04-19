CC=g++
SCAN_BUILD_DIR = scan-build-out
EXE=main

all: OrientationMath.o test.o
	$(CC) -o $(EXE) OrientationMath.o test.o
	
OrientationMath.o: OrientationMath.h
	$(CC) -Wall -Wextra -g -O0 -c OrientationMath.cpp
	
test.o: 
	$(CC) -Wall -Wextra -g -O0 -c test.cpp
	
clean:
	-rm *.o $(EXE)