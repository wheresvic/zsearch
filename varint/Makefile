
# $@ name of the target
# $^ name of all prerequisites with duplicates removed
# $< name of the first prerequisite

OSNAME  := $(shell uname -s)
CFLAGS_SIMPLE =  -std=gnu++0x -g -Wall -pedantic
#CFLAGS_SIMPLE = -flto -std=gnu++0x -g0 -Wall -Ofast -funroll-loops -pedantic
BUILD_DIR = ../build

LIBOBJECTS = $(BUILD_DIR)/varint_CompressedSet.o $(BUILD_DIR)/varint_CompressedDeltaChunk.o  $(BUILD_DIR)/varint_bitpacksimd.o $(BUILD_DIR)/varint_util.o $(BUILD_DIR)/varint_LazyAndSet.o $(BUILD_DIR)/varint_LazyOrSet.o $(BUILD_DIR)/varint_BasicSet.o

HEADERS = 


ifeq ($(OSNAME), Darwin)
CFLAGS = $(CFLAGS_SIMPLE)
LIBS = 
endif

ifeq ($(OSNAME), Linux)
CFLAGS = $(CFLAGS_SIMPLE) -msse2
LIBS = 
endif

# MINGW
ifneq (,$(findstring MINGW,$(OSNAME)))
CFLAGS = $(CFLAGS_SIMPLE)
LIBS = 
endif 

# Cygwin
ifneq (,$(findstring /cygdrive/,$(PATH)))
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
	$(CXX) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/varint_BasicSet.o : BasicSet.cpp 
	$(CXX) -c $(CFLAGS) $< -o $@
	
$(BUILD_DIR)/varint_CompressedDeltaChunk.o : CompressedDeltaChunk.cpp 
	$(CXX) -c $(CFLAGS) $< -o $@
	
$(BUILD_DIR)/varint_bitpacksimd.o : bitpacking/bitpacksimd.cpp 
	$(CXX) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/varint_bitpacksimd.o : bitpacking/simdintegratedbitpacking.c
	$(CXX) -c $(CFLAGS) $< -o $@
			
$(BUILD_DIR)/varint_util.o : bitpacking/util.cpp 
	$(CXX) -c $(CFLAGS) $< -o $@	

$(BUILD_DIR)/varint_LazyAndSet.o : LazyAndSet.cpp
	$(CXX) -c $(CFLAGS) $< -o $@	

$(BUILD_DIR)/varint_LazyOrSet.o : LazyOrSet.cpp
	$(CXX) -c $(CFLAGS) $< -o $@	
