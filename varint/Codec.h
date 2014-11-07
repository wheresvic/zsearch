#ifndef CODEC_H__
#define CODEC_H__
#include "bitpacking/compositecodec.h"
#include "bitpacking/simdbinarypacking.h"
#include "bitpacking/variablebyte.h"
#include "Source.h"
#include "Sink.h"
#include "CompressedDeltaChunk.h"
#include "bitpacking/util.h"
using namespace std;

class Codec {
private:
	CompositeCodec<SIMDBinaryPacking,VariableByte> codec;

 public:
    Codec(){
    }
    
    
    ~Codec(){
    }

    /**
     * @return the compressed size in bytes
     */
    template<typename srctype>
    __inline__ shared_ptr<CompressedDeltaChunk> Compress(const srctype src, size_t srcSize) const {
	   assert(!needPaddingTo128Bits(src));
     size_t nbyte = codec.compressedSize((const uint32_t *)src, (sizeof(*src)*srcSize) / 4);
     //shared_ptr<CompressedDeltaChunk> compblock(new CompressedDeltaChunk((sizeof(*src)*(srcSize+ 2048))));
     shared_ptr<CompressedDeltaChunk> compblock(new CompressedDeltaChunk(nbyte));

	   vector<uint8,cacheallocator>& v = compblock->getVector();
	   assert(!needPaddingTo128Bits(&v[0]));
	   size_t memavailable = v.size()/4;
	   codec.encodeArray((const uint32_t *)src, (sizeof(*src)*srcSize) / 4,(uint32_t *)&v[0], memavailable);
     //compblock->resize(memavailable*4);
     return compblock;
    }
    
    size_t Uncompress(Source& src, unsigned int* dst,size_t size) const  {
       assert(!needPaddingTo128Bits(dst));
    
    
       size_t sourceSize;
       const uint8* srcptr = src.Peek(&sourceSize);
       const uint32_t* srcptr2= (const uint32_t*)srcptr;
       assert(!needPaddingTo128Bits(srcptr2));
       size_t memavailable = size;
       codec.decodeArray(srcptr2, sourceSize/4,dst,memavailable);
       return memavailable*4;
    }

    bool findInArray(const unsigned int* array, size_t size,unsigned int target) const {       
       for(unsigned int idx = 0; idx<size; ++idx){
         unsigned int lastId = array[idx];
         if (lastId >= target)
            return (lastId == target);
       }
       return false;
    }

};

#endif  // CODEC_H__
