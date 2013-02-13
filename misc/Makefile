
# $@ name of the target
# $^ name of all prerequisites with duplicates removed
# $< name of the first prerequisite 

CC=g++
CFLAGS = -std=c++0x -Wall -O0 -pedantic -msse3 -I/usr/local/include
LIBS = -L/usr/local/lib
OBJECTS =

clean :
	rm -f *.o \
		test_compressedset

all: test_compressedset
	
test_compressedset: test_compressedset.cpp
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

