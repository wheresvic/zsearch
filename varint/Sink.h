// A Sink is an interface that consumes a sequence of bytes.
#ifndef SINK_H__
#define SINK_H__

#include <iostream>
#include <stdio.h>
#include <memory.h>

using namespace std;
class Sink {
private:
    //buffer's starting address
    char* dest_;
    //buffer total size
    size_t size_;
public:
    //helper variable
    char* currBytePtr;
    char* limit_;
    unsigned int numBytesWritten;
    
    template<size_t size>
    Sink(char (&a)[size]) : dest_(&a[0]), limit_(&a[0] + size){
        currBytePtr =   dest_;
        numBytesWritten = 0;
        size_ = size;
    }
    
    Sink(char* dest,size_t size) : dest_(dest) ,limit_(dest+size) {
        currBytePtr =   dest_;
        numBytesWritten = 0;
        size_ = size;
    }
    
   void resetBuffer(){
     currBytePtr=dest_;
     numBytesWritten=0; 
   }
    
    
    // Append "bytes[0,n-1]" to this
    __inline__ bool Append(const char* data, size_t n) {
      const size_t space_left = limit_ - currBytePtr;   
      if ( space_left < n){
        return false;
      }
      if (PREDICT_FALSE(data != currBytePtr)) {
        memcpy(currBytePtr, data, n);
      }
      currBytePtr += n;
      numBytesWritten +=n;
      return true;
    }
    
    size_t spaceLeft(){
        return limit_ - currBytePtr;
    }
    // Returns a writable buffer of the specified length for appending.
    // May return a pointer to the caller-owned scratch buffer which
    // must have at least the indicated length.  The returned buffer is
    // only valid until the next operation on this Sink
    //
    // After writing at most "length" bytes, call Append() with the
    // pointer returned from this function and the number of bytes
    // written. Many Append() implementations will avoid copying
    // bytes if this function returned an internal buffer.
    __inline__ char* GetAppendBuffer(size_t length, char* scratch) {
      const size_t space_left = limit_ - currBytePtr;
      if (space_left < length) {
        return  scratch;
      }
      return currBytePtr;
    }
    
   // Return the current output pointer so that a caller can see how
   // many bytes were produced. 
   char* CurrentDestination() const { return currBytePtr; }

   void flush(){
    
   }
//optionnal trait
   int getNumBytesWritten(){
    return numBytesWritten;
   }


};
#endif  // SINK_H__