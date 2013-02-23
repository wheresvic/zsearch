#ifndef  DELTA_CHUNK_STORE_H__
#define  DELTA_CHUNK_STORE_H__
#include <vector>
#include <iostream>
#include <unordered_map>
#include <memory>
#include "CompressedDeltaChunk.h"
using namespace std;

class DeltaChunkStore {
  vector<shared_ptr<CompressedDeltaChunk> > data2;
public:  
  DeltaChunkStore(){
  }

  shared_ptr<CompressedDeltaChunk> allocateBlock(size_t compressedSize){
    shared_ptr<CompressedDeltaChunk> compblock(new CompressedDeltaChunk(compressedSize));
    return compblock;
  }

  void add(const shared_ptr<CompressedDeltaChunk>& val) {
    data2.push_back(val);
  }

  const CompressedDeltaChunk& get(int index) const  {
    return *data2[index];
  }

  void compact(){
	if (data2.size() != data2.capacity()) {
        vector<shared_ptr<CompressedDeltaChunk> > tmp = data2;
        std::swap(data2, tmp);
    }
  }

  size_t size() const {
    return  data2.size();
  }

  int getSerialIntNum() const {
    int num = 1; // _len
    for(size_t i=0; i<data2.size(); i++)
    {
        num += 1 + (*data2[i]).getCompressedSize(); // 1 is the int to record the length of the array
    }
    return num;
  }

  void write(ostream & out) const{
    int size = data2.size();
    out.write((char*)&size,4);

    for(size_t i=0; i<data2.size(); i++)
    {
        (*data2[i]).write(out);
    }
  }

  void read(istream & in){
    int size = 0;
    in.read((char*)&size,4);
    data2.clear();
    for(int i = 0; i<size; i++){
      shared_ptr<CompressedDeltaChunk> compblock(new CompressedDeltaChunk(in));
 tcmalloc dynamiclib     data2.push_back(compblock);
    }
  }

  inline void swap(DeltaChunkStore & x)throw (){ // No throw exception guarantee
      using std::swap;
      swap(this->data2, x.data2);
  }

  friend void swap(DeltaChunkStore& lhs, DeltaChunkStore& rhs) noexcept
  {
     lhs.swap(rhs);
  }

};
#endif // DELTA_CHUNK_STORE_H__