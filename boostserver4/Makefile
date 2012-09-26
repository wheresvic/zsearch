
# $@ name of the target
# $^ name of all prerequisites with duplicates removed
# $< name of the first prerequisite 

CC=g++
CFLAGS = -std=c++0x -Wall -O0 -pedantic -I/usr/local/include/boost
BOOST_LIBS = -lboost_system -lboost_thread
OBJECTS =

clean :
	rm -f *.o \
		boostserver parse_post_input

parse_post_input : parse_post_input.cpp
	$(CC) -o $@ $^

all: boostserver

request.hpp : header.hpp

file_handler.o : file_handler.cpp file_handler.hpp mime_types.hpp reply.hpp request.hpp
	$(CC) -c $(CFLAGS) $<

mime_types.o : mime_types.cpp mime_types.hpp
	$(CC) -c $(CFLAGS) $<

reply.o : reply.cpp reply.hpp
	$(CC) -c $(CFLAGS) $<

request_parser.o : request_parser.cpp request_parser.hpp request.hpp coroutine.hpp yield.hpp unyield.hpp
	$(CC) -c $(CFLAGS) $<

server.o : server.cpp server.hpp request.hpp reply.hpp coroutine.hpp
	$(CC) -c $(CFLAGS) $<

boostserver: file_handler.o mime_types.o reply.o request_parser.o server.o main.cpp 
	$(CC) $(CFLAGS) -o $@ $^ $(BOOST_LIBS)

	
##
# Classes
##

#board.o : Board.h Board.cpp
#	$(CC) $(CFLAGS) -c Board.cpp

#g++ -c -O0 -g3 -Wall Promoter.cpp

