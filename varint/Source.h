// http://code.google.com/p/protobuf/source/browse/trunk/src/google/protobuf/io/zero_copy_stream_impl_lite.h
// http://code.google.com/p/protobuf/source/browse/trunk/src/google/protobuf/io/zero_copy_stream_impl_lite.cc

#ifndef  SOURCE_H__
#define  SOURCE_H__

#include "Common.h"
static const int kDefaultBlockSize = 8192;
// A Source is an interface that yields a sequence of bytes
// it's responsible for managing the buffers
class Source {
private:
      const  uint8* ptr_;
      size_t left_;
      size_t block_size_;
public:
    
    Source(const Source& src){
        ptr_ = src.ptr_;
        left_ = src.left_;
        block_size_ = src.block_size_;
        
    }
    
    template<typename A, size_t size>
    Source(A (&a)[size]){
        ptr_ = (uint8_t*)&a[0];
        left_ = sizeof(A) * size;
        block_size_ = left_;
    }


    // block_size define how many bytes to return at a time.
    template<typename A>
    Source(const A* p, size_t n,size_t block_size = 0) : 
         ptr_((uint8_t*)p),
         left_(sizeof(*p) * n){
            block_size_ = block_size > 0 ? block_size : left_;
    }

  
    // Return the number of bytes left to read from the source
    __inline__ size_t Available() const {
        return left_;
    }
    
    // To minimize the amount of data copying that needs to be done
    // return a pointer and size to internal buffer instead of copying to caller buffer.
    // The returned region is valid until the next call to Skip()
    // The caller is responsible for ensuring that it only reads uptop bytes_in_buffer
    __inline__ const uint8* Peek(size_t* bytes_in_buffer){
      *bytes_in_buffer = left_;
      return ptr_;
    }

    // Obtains a chunk of data from the stream.
    // If the returned value is false, there is no more data to return or an error occurred.
    // Otherwise, "size" points to the actual number of bytes read and "data" points to a pointer to a buffer containing these bytes.
    __inline__ bool Next(const void ** data,size_t * size){
        if (!left_) return false;
        
        if (left_ <= block_size_){
            Skip(left_);
        } else {
            Skip(block_size_);  
        }
        
        *data = Peek(size);
        return true;
    }

    // Skip the next n bytes
    // REQUIRES: Available() >= n
    // Returns false if the end of the stream is reached or some input error occurred.
    // In the end-of-stream case, the stream is advanced to the end of the stream
    // (so ByteCount() will return the total size of the stream).
    __inline__ bool Skip(size_t count){
      //if (count > left_) return false;
      left_ -= count;
      ptr_ += count;
      return true;
    }
};

#endif  // SOURCE_H__