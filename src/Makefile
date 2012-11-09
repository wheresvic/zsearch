
# $@ name of the target
# $^ name of all prerequisites with duplicates removed
# $< name of the first prerequisite

OSNAME  := $(shell uname -s)
ARCH := $(shell uname -m)

CC = g++
BUILD_DIR = ../build
LEVELDB_DIR = ../leveldb-1.7.0

BUILD_OBJECTS = $(BUILD_DIR)/DocumentImpl.o $(BUILD_DIR)/DocumentStoreSimple.o $(BUILD_DIR)/TokenizerImpl.o $(BUILD_DIR)/InvertedIndexImpl.o

OBJECTS = $(BUILD_OBJECTS) $(BUILD_DIR)/CompressedSet.a $(LEVELDB_DIR)/libleveldb.a

HEADERS = Constants.hpp Word.hpp Engine.hpp QueryParser.hpp

CFLAGS_PLAIN = -std=c++0x -Wall -O0 -pedantic -msse3 -I$(LEVELDB_DIR)/include -I..

ifeq ($(OSNAME), Darwin)
CFLAGS = $(CFLAGS_PLAIN) -I/opt/local/include 
LIBS = -L/opt/local/lib -lleveldb -lm -levent
endif

ifeq ($(OSNAME), Linux)
CFLAGS = $(CFLAGS_PLAIN) -I/usr/local/include -I/ms/dist/fsf/PROJ/libevent/2.0.19/include 
LIBS = -L/usr/local/lib -L/ms/dist/fsf/PROJ/libevent/2.0.19/lib -L$(LEVELDB_DIR) -lm -lpthread -levent
endif

clean :
	rm -f $(BUILD_OBJECTS) $(BUILD_DIR)/server $(BUILD_DIR)/engine

all: server engine

server: server.cpp $(HEADERS) $(OBJECTS)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$@ $^ $(LIBS)

engine: engine_simple_main.cpp $(HEADERS) $(OBJECTS)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$@ $^ $(LIBS)

##
# Classes
##

$(BUILD_DIR)/DocumentImpl.o : DocumentImpl.cpp DocumentImpl.h IDocument.h Constants.hpp
	$(CC) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/DocumentStoreSimple.o : DocumentStoreSimple.cpp DocumentStoreSimple.h IDocumentStore.h Constants.hpp
	$(CC) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/TokenizerImpl.o : TokenizerImpl.cpp TokenizerImpl.h ITokenizer.h Constants.hpp
	$(CC) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/InvertedIndexImpl.o : InvertedIndexImpl.cpp InvertedIndexImpl.h IInvertedIndex.h IKVStore.h
	$(CC) -c $(CFLAGS) $< -o $@
