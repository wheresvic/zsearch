#ifndef  COMPRESSED_DELTA_CHUNK_H__
#define  COMPRESSED_DELTA_CHUNK_H__
#include <vector>
#include "Common.h"
#include "Sink.h"
#include "Source.h"
#include "bitpacking/memutil.h"
class CompressedDeltaChunk {
private:
    vector<uint8,cacheallocator> data_;
    size_t compressedSize_;
    //disable copy constructor
    CompressedDeltaChunk(const CompressedDeltaChunk& other);
    CompressedDeltaChunk& operator=(const CompressedDeltaChunk& other);
public: 
	CompressedDeltaChunk();
	CompressedDeltaChunk(size_t compressedSize);
	CompressedDeltaChunk(istream & in);
	void resize(size_t newsize);
	vector<uint8,cacheallocator>& getVector();
	~CompressedDeltaChunk();
	size_t getCompressedSize();
	Sink getSink();
	Source getSource() const;
	void write(ostream & out) const;

}__attribute__ ((aligned (256)));
#endif // COMPRESSED_DELTA_CHUNK_H__