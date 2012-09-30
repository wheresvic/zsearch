
# $@ name of the target
# $^ name of all prerequisites with duplicates removed
# $< name of the first prerequisite 

CC=g++
CFLAGS = -std=c++0x -Wall -O0 -pedantic -msse3 -I/usr/local/include -I/ms/dist/fsf/PROJ/libevent/2.0.19/include
LIBS = -L/usr/local/lib -L/ms/dist/fsf/PROJ/libevent/2.0.19/lib -levent -lm 
OBJECTS =

clean :
	rm -f *.o \
		server engine

all: server engine
	
server: server.cpp
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

DocumentImpl.o : DocumentImpl.cpp DocumentImpl.h IDocument.h
	$(CC) -c $(CFLAGS) $<

DocumentIndexImpl.o : DocumentIndexImpl.cpp DocumentIndexImpl.h IDocumentIndex.h
	$(CC) -c $(CFLAGS) $<

engine: engine.cpp QueryParser.hpp Tokenizer.hpp DocumentImpl.o DocumentIndexImpl.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
	
##
# Classes
##

#board.o : Board.h Board.cpp
#	$(CC) $(CFLAGS) -c Board.cpp

#g++ -c -O0 -g3 -Wall Promoter.cpp

