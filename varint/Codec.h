#ifndef CODEC_H__
#define CODEC_H__
#include "bitpacking/compositecodec.h"
#include "bitpacking/simdbinarypacking.h"
#include "bitpacking/variablebyte.h"
#include "Source.h"
#include "Sink.h"
#include "CompressedDeltaChunk.h"
using namespace std;

class Codec {
private:
	CompositeCodec<SIMDBinaryPacking,VariableByte> codec;

 public:
	Codec();
    
	~Codec();

       
    //Code below is part of the public interface

	bool findInDeltaArray(unsigned int array[],size_t size,unsigned int target) const;
	double diffclock(clock_t clock1,clock_t clock2) const;
    size_t Uncompress(Source& src, unsigned int* dst,size_t size) const;

    /**
     * @return the compressed size in bytes
     */
    template<typename srctype>
    __inline__ shared_ptr<CompressedDeltaChunk> Compress(srctype src, size_t srcSize) const {
       shared_ptr<CompressedDeltaChunk> compblock(new CompressedDeltaChunk(sizeof(*src)*(srcSize + 2048)));
	   vector<uint8,cacheallocator>& v = compblock->getVector();
	   size_t memavailable = v.size()/4;
	   codec.encodeArray((const uint32_t *)src, (sizeof(*src)*srcSize) / 4,(uint32_t *)&v[0], memavailable);
       compblock->resize(memavailable*4);
       return compblock;
    }
};

#endif  // CODEC_H__