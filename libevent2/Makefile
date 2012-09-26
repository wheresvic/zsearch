
CC=g++
CFLAGS = -std=c++0x -Wall -O0 -pedantic -I/usr/local/include
LIBS = -L/usr/local/lib -levent -lm 
OBJECTS =

clean :
	rm -f *.o \
		server

all: server
	
server: server.cpp
	$(CC) $(CFLAGS) -o server server.cpp $(LIBS)

##
# Classes
##

#board.o : Board.h Board.cpp
#	$(CC) $(CFLAGS) -c Board.cpp

#g++ -c -O0 -g3 -Wall Promoter.cpp

