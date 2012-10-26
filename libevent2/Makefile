
# $@ name of the target
# $^ name of all prerequisites with duplicates removed
# $< name of the first prerequisite

OSNAME  := $(shell uname -s)

CC = g++
CFLAGS_SIMPLE = -std=c++0x -Wall -O0 -pedantic
BUILD_DIR = ../build
OBJECTS = $(BUILD_DIR)/DocumentImpl.o $(BUILD_DIR)/DocumentStoreImpl.o $(BUILD_DIR)/TokenizerImpl.o
HEADERS = Constants.hpp Word.hpp Engine.hpp QueryParser.hpp


ifeq ($(OSNAME), Darwin)
CFLAGS =  -std=gnu++0x -Wall -O3 -pedantic  -I/opt/local/include 
LIBS = -L/opt/local/lib -levent -lm
endif

ifeq ($(OSNAME), Linux)
CFLAGS = -std=c++0x -Wall -O0 -pedantic -msse3 -I/usr/local/include -I/ms/dist/fsf/PROJ/libevent/2.0.19/include
LIBS = -L/usr/local/lib -L/ms/dist/fsf/PROJ/libevent/2.0.19/lib -levent -lm
endif


clean :
	rm -f ../build/*.o *.o \
		server engine tests

all: server engine tests

tests: unittests.cpp $(HEADERS) $(OBJECTS)
	$(CC) $(CFLAGS_SIMPLE) -o $@ $^

server: server.cpp $(HEADERS) $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

engine: engine_simple_main.cpp $(HEADERS) $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)


##
# Classes
##

$(BUILD_DIR)/DocumentImpl.o : DocumentImpl.cpp DocumentImpl.h IDocument.h Constants.hpp
	$(CC) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/DocumentStoreImpl.o : DocumentStoreImpl.cpp DocumentStoreImpl.h IDocumentStore.h Constants.hpp
	$(CC) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/TokenizerImpl.o : TokenizerImpl.cpp TokenizerImpl.h ITokenizer.h Constants.hpp
	$(CC) -c $(CFLAGS) $< -o $@

