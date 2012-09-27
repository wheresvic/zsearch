
# $@ name of the target
# $^ name of all prerequisites with duplicates removed
# $< name of the first prerequisite 

CC=g++
CFLAGS = -std=c++0x -Wall -O0 -pedantic -I/usr/local/include -I/ms/dist/fsf/PROJ/libevent/2.0.19/include
LIBS = -L/usr/local/lib -L/ms/dist/fsf/PROJ/libevent/2.0.19/lib -levent -lm 
OBJECTS =

clean :
	rm -f *.o \
		server query_parser

all: server query_parser engine
	
server: server.cpp
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

query_parser: query_parser.cpp QueryParser.hpp Tokenizer.hpp
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

engine: engine.cpp QueryParser.hpp Tokenizer.hpp Document.hpp
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
	
##
# Classes
##

#board.o : Board.h Board.cpp
#	$(CC) $(CFLAGS) -c Board.cpp

#g++ -c -O0 -g3 -Wall Promoter.cpp

