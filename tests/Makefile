
# $@ name of the target
# $^ name of all prerequisites with duplicates removed
# $< name of the first prerequisite

OSNAME  := $(shell uname -s)
ARCH := $(shell uname -m)

CC = g++

BUILD_DIR_TEST = ../build/tests
BUILD_DIR = ../build
LEVELDB_DIR = ../leveldb-1.7.0

OBJECTS =  $(BUILD_DIR)/CompressedSet.a $(LEVELDB_DIR)/libleveldb.a

CFLAGS_PLAIN = -std=gnu++0x -Wall -O3 -pedantic -msse3 -I$(LEVELDB_DIR)/include -I../lib -I../lib/rapidxml-1.13 -I..

ifeq ($(OSNAME), Darwin)
CFLAGS = $(CFLAGS_PLAIN) -I/opt/local/include
LIBS = -L/opt/local/lib -L/usr/local/lib -lleveldb -lm -levent
endif

ifeq ($(OSNAME), Linux)
CFLAGS = $(CFLAGS_PLAIN) -I/usr/local/include -I/ms/dist/fsf/PROJ/libevent/2.0.19/include -D_GLIBCXX_USE_NANOSLEEP
LIBS = -L/usr/local/lib -L/ms/dist/fsf/PROJ/libevent/2.0.19/lib -L../leveldb-1.7.0 -lm -levent -lpthread
endif


clean:
	rm -f $(BUILD_DIR_TEST)/*

all: memory_leak_test thread_test document_test tokenizer_test xml_test inverted_index_test compressed_set_test basic_set_test statistics_test

memory_leak_test: memory_leak_test.cpp $(OBJECTS)
	$(CXX) $(CFLAGS) -o $(BUILD_DIR_TEST)/$@ $^ $(LIBS)

thread_test: thread_test.cpp $(OBJECTS)
	$(CXX) $(CFLAGS) -o $(BUILD_DIR_TEST)/$@ $^ $(LIBS)

document_test: document_test.cpp $(OBJECTS)
	$(CXX) $(CFLAGS) -o $(BUILD_DIR_TEST)/$@ $^ $(LIBS)

tokenizer_test: tokenizer_test.cpp $(OBJECTS)
	$(CXX) $(CFLAGS) -o $(BUILD_DIR_TEST)/$@ $^ $(LIBS)

xml_test: xml_test.cpp $(OBJECTS)
	$(CXX) $(CFLAGS) -o $(BUILD_DIR_TEST)/$@ $^ $(LIBS)

inverted_index_test: inverted_index_test.cpp $(OBJECTS)
	$(CXX) $(CFLAGS) -o $(BUILD_DIR_TEST)/$@ $^ $(LIBS)

compressed_set_test: CompressedSetTests.cpp $(OBJECTS)
	$(CXX) $(CFLAGS) -o $(BUILD_DIR_TEST)/$@ $^ $(LIBS)
	
compressed_set_test2: CompressedSet_test.cpp $(OBJECTS)
	$(CXX) $(CFLAGS) -o $(BUILD_DIR_TEST)/$@ $^ $(LIBS)

basic_set_test: BasicSetTest.cpp $(OBJECTS)
	$(CXX) $(CFLAGS) -o $(BUILD_DIR_TEST)/$@ $^ $(LIBS)

statistics_test: statistics_test.cpp $(OBJECTS)
	$(CXX) $(CFLAGS) -o $(BUILD_DIR_TEST)/$@ $^ $(LIBS)
	
