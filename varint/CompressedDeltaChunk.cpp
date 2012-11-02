#include "CompressedDeltaChunk.h"
#include <vector>
#include "Common.h"
#include "Sink.h"
#include "Source.h"
#include "bitpacking/memutil.h"

    CompressedDeltaChunk::CompressedDeltaChunk(){
        compressedSize_ = 0; 
    }

    CompressedDeltaChunk::CompressedDeltaChunk(size_t compressedSize):data_(compressedSize){
        compressedSize_ = compressedSize;
    }
    
    CompressedDeltaChunk::CompressedDeltaChunk(istream & in) :compressedSize_(0) {
        in.read((char*)&compressedSize_,4);
		data_.resize(compressedSize_);
        in.read((char*)&(data_[0]),compressedSize_);
        
    }

    void CompressedDeltaChunk::resize(size_t newsize){
		compressedSize_ = newsize;
		data_.resize(newsize);
	    vector<uint8,cacheallocator> tmp(data_);
	    swap(data_, tmp);
    }

    vector<uint8,cacheallocator>& CompressedDeltaChunk::getVector(){
		return data_;
    }

    CompressedDeltaChunk::~CompressedDeltaChunk(){
    }
    
    
    size_t CompressedDeltaChunk::getCompressedSize(){
        return compressedSize_;
    }
    
    Sink CompressedDeltaChunk::getSink(){
        return Sink((char*)&(data_[0]),compressedSize_);
    }
    
    Source CompressedDeltaChunk::getSource() const {
        return Source((char*)&(data_[0]),compressedSize_);
    }
    
    void CompressedDeltaChunk::write(ostream & out) const{
        out.write((char*)&compressedSize_,4);
        out.write((char*)&(data_[0]),compressedSize_);
    }