
# $@ name of the target
# $^ name of all prerequisites with duplicates removed
# $< name of the first prerequisite
OSNAME  := $(shell uname -s)
ifeq ($(OSNAME), Darwin)
CC=g++
CFLAGS_SIMPLE = -std=gnu++0x -Wall -O3 -pedantic
CFLAGS =  -std=gnu++0x -Wall -O3 -pedantic  -I/opt/local/include 
LIBS = -L/opt/local/lib -levent -lm
OBJECTS =
endif
ifeq ($(OSNAME), Linux)
CC=g++
CFLAGS_SIMPLE = -std=c++0x -Wall -O0 -pedantic
CFLAGS = -std=c++0x -Wall -O0 -pedantic -msse3 -I/usr/local/include -I/ms/dist/fsf/PROJ/libevent/2.0.19/include
LIBS = -L/usr/local/lib -L/ms/dist/fsf/PROJ/libevent/2.0.19/lib -levent -lm
OBJECTS =
endif


clean :
	rm -f *.o \
		server engine tests

all: server engine tests

tests: unittests.cpp QueryParser.hpp Constants.hpp
	$(CC) $(CFLAGS_SIMPLE) -o $@ $^

server: server.cpp
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

engine: engine_simple_main.cpp EngineSet.hpp QueryParser.hpp Tokenizer.hpp DocumentImpl.o DocumentIndexImpl.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)


##
# Classes
##

DocumentImpl.o : DocumentImpl.cpp DocumentImpl.h IDocument.h
	$(CC) -c $(CFLAGS) $<

DocumentIndexImpl.o : DocumentIndexImpl.cpp DocumentIndexImpl.h IDocumentIndex.h
	$(CC) -c $(CFLAGS) $<
