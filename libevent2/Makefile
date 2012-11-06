
# $@ name of the target
# $^ name of all prerequisites with duplicates removed
# $< name of the first prerequisite

OSNAME  := $(shell uname -s)
ARCH := $(shell uname -m)

CC = g++
BUILD_DIR = ../build
LEVELDB_DIR = ../leveldb-1.7.0
OBJECTS = $(BUILD_DIR)/DocumentImpl.o $(BUILD_DIR)/DocumentStoreSimple.o $(BUILD_DIR)/TokenizerImpl.o $(BUILD_DIR)/CompressedSet.a $(LEVELDB_DIR)/libleveldb.a
HEADERS = Constants.hpp Word.hpp Engine.hpp QueryParser.hpp KVStoreInMemory.hpp


ifeq ($(OSNAME), Darwin)
CFLAGS = -std=gnu++0x -Wall -O3 -pedantic  -I/opt/local/include -I../leveldb-1.7.0/include
LIBS = -L/opt/local/lib -lleveldb -lm -levent
endif

ifeq ($(OSNAME), Linux)
CFLAGS = -std=c++0x -Wall -O0 -pedantic -msse3 -I/usr/local/include -I/ms/dist/fsf/PROJ/libevent/2.0.19/include -I../leveldb-1.7.0/include
LIBS = -L/usr/local/lib -L/ms/dist/fsf/PROJ/libevent/2.0.19/lib -L../leveldb-1.7.0 -lm -lpthread -levent
endif


clean :
	rm -f ../build/*.o *.o \
		server engine tests

all: server engine tests

tests: unittests.cpp $(HEADERS) $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

server: server.cpp $(HEADERS) $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

engine: engine_simple_main.cpp $(HEADERS) $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

##
# Classes
##

$(BUILD_DIR)/DocumentImpl.o : DocumentImpl.cpp DocumentImpl.h IDocument.h Constants.hpp
	$(CC) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/DocumentStoreSimple.o : DocumentStoreSimple.cpp DocumentStoreSimple.h IDocumentStore.h Constants.hpp
	$(CC) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/TokenizerImpl.o : TokenizerImpl.cpp TokenizerImpl.h ITokenizer.h Constants.hpp
	$(CC) -c $(CFLAGS) $< -o $@

