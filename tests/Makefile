
# $@ name of the target
# $^ name of all prerequisites with duplicates removed
# $< name of the first prerequisite

OSNAME  := $(shell uname -s)
ARCH := $(shell uname -m)

CC = g++

BUILD_DIR_TEST = ../build/tests
BUILD_DIR = ../build
LEVELDB_DIR = ../leveldb-1.7.0

OBJECTS = $(BUILD_DIR)/DocumentImpl.o $(BUILD_DIR)/DocumentStoreSimple.o $(BUILD_DIR)/TokenizerImpl.o $(BUILD_DIR)/KVStoreInMemory.o $(BUILD_DIR)/KVStoreLevelDb.o $(BUILD_DIR)/CompressedSet.a $(LEVELDB_DIR)/libleveldb.a

CFLAGS_PLAIN = -std=gnu++0x -Wall -O3 -pedantic -msse3 -I$(LEVELDB_DIR)/include -I..

ifeq ($(OSNAME), Darwin)
CFLAGS = $(CFLAGS_PLAIN) -I/opt/local/include
LIBS = -L/opt/local/lib -lleveldb -lm -levent
endif

ifeq ($(OSNAME), Linux)
CFLAGS = $(CFLAGS_PLAIN) -I/usr/local/include -I/ms/dist/fsf/PROJ/libevent/2.0.19/include -D_GLIBCXX_USE_NANOSLEEP
LIBS = -L/usr/local/lib -L/ms/dist/fsf/PROJ/libevent/2.0.19/lib -L../leveldb-1.7.0 -lm -levent -lpthread
endif


clean :
	rm -f $(BUILD_DIR_TEST)/*

all: document_test query_parser_test xml_test inverted_index_test compressed_set_test basic_set_test statistics_test thread_test

thread_test : thread_test.cpp $(OBJECTS)
	$(CC) $(CFLAGS) -o $(BUILD_DIR_TEST)/$@ $^ $(LIBS)

query_parser_test : query_parser_test.cpp $(OBJECTS)
	$(CC) $(CFLAGS) -o $(BUILD_DIR_TEST)/$@ $^ $(LIBS)

xml_test : xml_test.cpp $(OBJECTS)
	$(CC) $(CFLAGS) -o $(BUILD_DIR_TEST)/$@ $^ $(LIBS)

inverted_index_test : inverted_index_test.cpp $(OBJECTS)
	$(CC) $(CFLAGS) -o $(BUILD_DIR_TEST)/$@ $^ $(LIBS)

compressed_set_test : CompressedSetTests.cpp $(OBJECTS)
	$(CC) $(CFLAGS) -o $(BUILD_DIR_TEST)/$@ $^ $(LIBS)

basic_set_test : BasicSetTest.cpp $(OBJECTS)
	$(CC) $(CFLAGS) -o $(BUILD_DIR_TEST)/$@ $^ $(LIBS)

statistics_test : statistics_test.cpp $(OBJECTS)
	$(CC) $(CFLAGS) -o $(BUILD_DIR_TEST)/$@ $^ $(LIBS)
	
