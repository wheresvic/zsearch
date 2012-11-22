
# $@ name of the target
# $^ name of all prerequisites with duplicates removed
# $< name of the first prerequisite

OSNAME  := $(shell uname -s)

CC = g++
CFLAGS_SIMPLE = -std=gnu++0x -Wall -O3 -pedantic
BUILD_DIR = ../build

LIBOBJECTS = $(BUILD_DIR)/varint_CompressedSet.o $(BUILD_DIR)/varint_CompressedDeltaChunk.o $(BUILD_DIR)/varint_Codec.o $(BUILD_DIR)/varint_bitpacksimd.o $(BUILD_DIR)/varint_util.o $(BUILD_DIR)/varint_LazyAndSet.o $(BUILD_DIR)/varint_LazyOrSet.o $(BUILD_DIR)/varint_BasicSet.o

HEADERS = 


ifeq ($(OSNAME), Darwin)
CFLAGS = $(CFLAGS_SIMPLE)
LIBS = 
endif

ifeq ($(OSNAME), Linux)
CFLAGS = $(CFLAGS_SIMPLE) -msse2
LIBS = 
endif


clean :
	rm -f $(LIBOBJECTS) *.o $(BUILD_DIR)/CompressedSet.a

all: CompressedSet.a

CompressedSet.a : $(LIBOBJECTS)
	$(AR) -rs $(BUILD_DIR)/$@ $(LIBOBJECTS)


##
# Classes
##

$(BUILD_DIR)/varint_CompressedSet.o : CompressedSet.cpp 
	$(CC) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/varint_BasicSet.o : BasicSet.cpp 
	$(CC) -c $(CFLAGS) $< -o $@
	
$(BUILD_DIR)/varint_CompressedDeltaChunk.o : CompressedDeltaChunk.cpp 
	$(CC) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/varint_Codec.o : Codec.cpp 
	$(CC) -c $(CFLAGS) $< -o $@ 
	
$(BUILD_DIR)/varint_bitpacksimd.o : bitpacking/bitpacksimd.cpp 
	$(CC) -c $(CFLAGS) $< -o $@
			
$(BUILD_DIR)/varint_util.o : bitpacking/util.cpp 
	$(CC) -c $(CFLAGS) $< -o $@	

$(BUILD_DIR)/varint_LazyAndSet.o : LazyAndSet.cpp
	$(CC) -c $(CFLAGS) $< -o $@	

$(BUILD_DIR)/varint_LazyOrSet.o : LazyOrSet.cpp
	$(CC) -c $(CFLAGS) $< -o $@	
