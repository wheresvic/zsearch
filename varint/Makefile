
# $@ name of the target
# $^ name of all prerequisites with duplicates removed
# $< name of the first prerequisite

OSNAME  := $(shell uname -s)

CC = g++
CFLAGS_SIMPLE = -std=c++0x -Wall -O0 -pedantic
BUILD_DIR = ../build
LIBOBJECTS = $(BUILD_DIR)/varint_CompressedSet.o $(BUILD_DIR)/varint_CompressedDeltaChunk.o $(BUILD_DIR)/varint_Codec.o $(BUILD_DIR)/varint_bitpacksimd.o $(BUILD_DIR)/varint_util.o
HEADERS = 


ifeq ($(OSNAME), Darwin)
CFLAGS =  -std=gnu++0x -Wall -O3 -pedantic  
LIBS = 
endif

ifeq ($(OSNAME), Linux)
CFLAGS = -std=c++0x -Wall -O0 -pedantic -msse2
LIBS = 
endif


clean :
	rm -f ../build/*.o *.o \
		lib

all: $(BUILD_DIR)/CompressedSet.a



##
# Classes
##
$(BUILD_DIR)/CompressedSet.a : $(LIBOBJECTS)
	$(AR) -rs $@ $(LIBOBJECTS)

$(BUILD_DIR)/varint_CompressedSet.o : ../varint/CompressedSet.cpp 
	$(CC) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/varint_CompressedDeltaChunk.o : ../varint/CompressedDeltaChunk.cpp 
	$(CC) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/varint_Codec.o : ../varint/Codec.cpp 
	$(CC) -c $(CFLAGS) $< -o $@ 
	
$(BUILD_DIR)/varint_bitpacksimd.o : ../varint/bitpacking/bitpacksimd.cpp 
	$(CC) -c $(CFLAGS) $< -o $@
			
$(BUILD_DIR)/varint_util.o : ../varint/bitpacking/util.cpp 
	$(CC) -c $(CFLAGS) $< -o $@	

