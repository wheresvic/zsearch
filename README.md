# **zsearch** A high performance search engine

Low Data fragmentation and good random write performance by using levelDB Log Structured Merge Trees. High performance query speed by using CompressedBitmap to store DocumentIds in an InvertedIndex interface provided by a simple libEvent2 http server.

## Project Design

	Engine 
		-> tokenizer	
		-> documentStore
		-> invertedIndex
			-> KVStore
				-> InMemory
				-> LevelDb
			-> setFactory
		-> setFactory
		-> wordIndex

## Project Organization

TODO

## Contact

-   [Homepage] (http://victorparmar.github.com/zsearch/)
-   Victor at victorparmar@gmail.com
-   Maxime at maximecaron@gmail.com

## Dependencies

All of the following:

-	[g++] (http://gcc.gnu.org/) >= 4.7.2
-	[libevent2] (http://libevent.org/) >= 2.0.19
-   [Python](http://python.org/) >= 2.7

## Cloning and Running

You can clone this repo and simply execute:

    git clone git://github.com/victorparmar/zsearch.git
    cd zsearch
    ./buildall.sh
	./build/server ./src

## License

-   Mozilla


