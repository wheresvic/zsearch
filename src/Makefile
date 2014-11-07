# $@ name of the target
# $^ name of all prerequisites with duplicates removed
# $< name of the first prerequisite

OSNAME := $(shell uname -s)
ARCH := $(shell uname -m)

CC = g++
BUILD_DIR = ../build
LEVELDB_DIR = ../leveldb


BUILD_OBJECTS =

OBJECTS = $(BUILD_OBJECTS) $(BUILD_DIR)/CompressedSet.a $(LEVELDB_DIR)/libleveldb.a 

HEADERS = TokenizerImpl.hpp Constants.hpp ZUtil.hpp Statistics.hpp Engine.hpp InvertedIndexImpl.hpp InvertedIndexBatch.hpp DocumentImpl.hpp InvertedIndexSimpleBatch.hpp DocumentKVStore.hpp ZException.hpp ZUtil.hpp WordIndexKVStore.hpp KVStoreLevelDb.hpp IDocument.h IInvertedIndex.h IKVStore.h KVStoreInMemory.hpp NameSpaceKVStore.hpp FieldKVStore.hpp EngineDataKVStore.hpp

CFLAGS_PLAIN =  -flto -std=gnu++0x -Wall -g0 -Ofast -funroll-loops -msse2 -I$(LEVELDB_DIR)/include -I../lib -I../lib/rapidxml-1.13 -I.. -D_GLIBCXX_USE_NANOSLEEP

ifeq ($(OSNAME), Darwin)
CFLAGS = $(CFLAGS_PLAIN) -I/usr/local/include -I/opt/local/include -DOS_MACOSX -DINTEL64
LIBS = -L/usr/local/lib -L/opt/local/lib -L$(LEVELDB_DIR) -lleveldb -lm -levent
endif


ifeq ($(OSNAME), Linux)
CFLAGS = $(CFLAGS_PLAIN) -I/usr/local/include -I/ms/dist/fsf/PROJ/libevent/2.0.19/include
LIBS = -L/usr/local/lib -L/ms/dist/fsf/PROJ/libevent/2.0.19/lib -L$(LEVELDB_DIR) -lm -lpthread -levent 
endif

clean :
	rm -f $(BUILD_OBJECTS) $(BUILD_DIR)/server $(BUILD_DIR)/engine

all: server engine

server: server.cpp $(OBJECTS)
	$(CXX) $(CFLAGS) -o $(BUILD_DIR)/$@ $^ $(LIBS)

engine: engine_simple_main.cpp $(OBJECTS)
	$(CXX) $(CFLAGS) -o $(BUILD_DIR)/$@ $^ $(LIBS) 
	# -ljemalloc

##
# Classes
##

#$(BUILD_DIR)/TokenizerImpl.o : TokenizerImpl.cpp TokenizerImpl.h ITokenizer.h Constants.hpp
#	$(CXX) -c $(CFLAGS) $< -o $@


