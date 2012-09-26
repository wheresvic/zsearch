#ifndef CODEC_H__
#define CODEC_H__
#include "Common.h"
#include "CompressedDeltaChunk.h"
#include "Source.h"
#include "Sink.h"
#include <vector>
#include "bitpacking/compositecodec.h"
#include "bitpacking/simdbinarypacking.h"

using namespace std;

class Codec {
private:
	CompositeCodec<SIMDBinaryPacking,JustCopy> codec;
//	JustCopy codec;
 public:
    Codec(){
    }
    
    
    ~Codec(){
    }

       
    //Code below is part of the public interface

    __inline__ bool findInDeltaArray(unsigned int array[],size_t size,unsigned int target) const {       
       unsigned int lastId = array[0];
       if (lastId == target) return true;
       // searching while doing prefix sum (to get docIds instead of d-gaps)
       for(unsigned int idx = 1; idx<size; ++idx){
         lastId += (array[idx]+1);
         if (lastId >= target)
            return (lastId == target);
       }
       return false;
    }
	double diffclock(clock_t clock1,clock_t clock2) const
	{
		double diffticks=clock1-clock2;
		double diffms=(diffticks*1000)/CLOCKS_PER_SEC;
		return diffms;
	}
    __inline__ size_t Uncompress(Source& src, unsigned int* dst,size_t size) const  {
	   
	   size_t sourceSize;
	   const uint8* srcptr = src.Peek(&sourceSize);
	   const uint32_t* srcptr2= (const uint32_t*)srcptr;
	   size_t memavailable = size;
	   codec.decodeArray(srcptr2, sourceSize/4,dst,memavailable);
       return memavailable*4;
    }

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