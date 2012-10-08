//based on https://github.com/maximecaron/kamikaze/blob/master/src/main/java/com/kamikaze/docidset/impl/PForDeltaDocIdSet.java
#ifndef COMPRESSED_SET_H__
#define COMPRESSED_SET_H__

#define  BLOCK_SIZE_BIT 11
// should be a 2 ^ BLOCK_SIZE_BIT
// 256 should give good cache alignement
#define  DEFAULT_BATCH_SIZE 2048
// should be DEFAULT_BATCH_SIZE -1
#define  BLOCK_SIZE_MODULO 2047
//int i=-1;
//for(int x=DEFAULT_BATCH_SIZE; x>0; ++i, x = x>> 1) { }
//BLOCK_INDEX_SHIFT_BITS = i;
#define BLOCK_INDEX_SHIFT_BITS 11

#include <iostream>
#include <stdio.h>

#include <assert.h>
#include "Common.h"
#include "Set.h"
#include "Codec.h"
#include "DeltaChunkStore.h"
#include "CompressedDeltaChunk.h"
#include "Source.h"
#include "Sink.h"
#include <vector>
#include <memory>
#include "CollectionHelper.h"

using namespace std;
const int NO_MORE_DOCS = std::numeric_limits<int>::max();
class CompressedSet;

class CompressedSet : public Set {
public:
    class Iterator : public Set::Iterator {
	    int lastAccessedDocId; 
        int cursor; // the current pointer of the input 
        unsigned int totalDocIdNum;
       
        int compBlockNum; // the number of compressed blocks
        unsigned int*  iterDecompBlock; // temporary storage for the decompressed data
		unsigned int* currentNoCompBlock;

        //parent
        const CompressedSet* set;
        //int BLOCK_INDEX_SHIFT_BITS; // floor(log(blockSize))
        int advanceToTargetInTheFollowingCompBlocksNoPostProcessing(int target, int startBlockIndex);
        int getBlockIndex(int docIdIndex);
    public:
        Iterator(const CompressedSet* parentSet);
        Iterator(const CompressedSet::Iterator& other);
        // assignator operator disabled for now
        CompressedSet::Iterator& operator=(const CompressedSet::Iterator& rhs);
        ~Iterator();
    
        __inline__ int docID();
        __inline__ int nextDoc();
        int Advance(int target);
    };
private:    
    unsigned int sizeOfCurrentNoCompBlock; // the number of uncompressed elements that is hold in the currentNoCompBlock
    unsigned int compressedByteSize;    
    // Two separate arrays containing
    // the last docID 
    // of each block in words in uncompressed form.
    vector<unsigned int> baseListForOnlyCompBlocks;
    unsigned int* myDecompBlock;
	const CompressedSet& operator=(const CompressedSet& other);


public:
	unsigned int lastAdded; // recently inserted/accessed element   
    Codec codec; // varint encoding codec    
    unsigned int totalDocIdNum; // the total number of elemnts that have been inserted/accessed so far  
    unsigned int* currentNoCompBlock;  // the memory used to store the uncompressed elements. Once the block is full, all its elements are compressed into sequencOfCompBlock and the block is cleared.
    DeltaChunkStore sequenceOfCompBlocks; // Store for list compressed delta chunk 
    
    
    CompressedSet(const CompressedSet& other){
	    assert(totalDocIdNum <= 1); // You are trying to copy the bitmap, a terrible idea in general, for performance reasons
		
        myDecompBlock = new unsigned int[DEFAULT_BATCH_SIZE];
        currentNoCompBlock = new unsigned int[DEFAULT_BATCH_SIZE];
        lastAdded = other.lastAdded;
        //compressedByteSize = other.compressedByteSize;
        sizeOfCurrentNoCompBlock = other.sizeOfCurrentNoCompBlock;
        totalDocIdNum = other.totalDocIdNum;
        memcpy( myDecompBlock,other.myDecompBlock, sizeof(unsigned int)*DEFAULT_BATCH_SIZE);
        memcpy( currentNoCompBlock,other.currentNoCompBlock, sizeof(unsigned int)*DEFAULT_BATCH_SIZE );	
    }


   /**
    * Swap the content of this bitmap with another bitmap.
    * No copying is done. (Running time complexity is constant.)
    */

    void swap(CompressedSet & x){
	 //todo: implement it 
     assert(false); 
    }

    
    CompressedSet(){
        myDecompBlock = new unsigned int[DEFAULT_BATCH_SIZE];
        currentNoCompBlock = new unsigned int[DEFAULT_BATCH_SIZE];
        lastAdded = 0;
        //compressedByteSize = 0;
        sizeOfCurrentNoCompBlock = 0;
        totalDocIdNum = 0;
        initSet();
    }
    
    ~CompressedSet(){
        delete[] currentNoCompBlock;
        delete[] myDecompBlock;
    }


    void write(ostream & out) const {
        out.write((char*)&lastAdded,4);
        out.write((char*)&totalDocIdNum,4);
               

        //write base (skipping info)
        int baseListForOnlyCompBlocksSize = baseListForOnlyCompBlocks.size();
        out.write((char*)&baseListForOnlyCompBlocksSize,4);
        out.write((char*)&baseListForOnlyCompBlocks[0],baseListForOnlyCompBlocksSize*4);
	     

        //write the last block (uncompressed) 
        out.write((char*)&sizeOfCurrentNoCompBlock,4);
        out.write((char*)&currentNoCompBlock[0],sizeOfCurrentNoCompBlock*4);
        
        //write compressed blocks
        sequenceOfCompBlocks.write(out);
        out.flush();    
    }

    void read(istream & in)  {
        memset(myDecompBlock, 0, DEFAULT_BATCH_SIZE*4);
        memset(currentNoCompBlock, 0, DEFAULT_BATCH_SIZE*4);

        //read lastAdded
        in.read((char*)&lastAdded,4);
        //read lastAdded
        in.read((char*)&totalDocIdNum,4);
        
        //read base (skipping info)
        int baseListForOnlyCompBlocksSize = 0;
        in.read((char*)&baseListForOnlyCompBlocksSize,4);
        baseListForOnlyCompBlocks.clear();
        baseListForOnlyCompBlocks.resize(baseListForOnlyCompBlocksSize);
        in.read((char*)&baseListForOnlyCompBlocks[0],baseListForOnlyCompBlocksSize*4);

        //read the last block (uncompressed) 
        in.read((char*)&sizeOfCurrentNoCompBlock,4);
        in.read((char*)&currentNoCompBlock[0],sizeOfCurrentNoCompBlock*4);

        //write compressed blocks
        sequenceOfCompBlocks.read(in);
    }

    shared_ptr<Set::Iterator>  iterator() const {
	   shared_ptr<Set::Iterator> it( new CompressedSet::Iterator(this));
       return it;
    }


    /**
     * Add an array of sorted docIds to the set
     */
    void addDocs(unsigned int docids[],size_t start,size_t len){
      if ((len + sizeOfCurrentNoCompBlock) <= DEFAULT_BATCH_SIZE) {
        memcpy( &currentNoCompBlock[sizeOfCurrentNoCompBlock],&docids[start], len*4 );
        sizeOfCurrentNoCompBlock += len;
      } else {      
         // the first block can be completed so fillup a complet block
         int copyLen = DEFAULT_BATCH_SIZE - sizeOfCurrentNoCompBlock;
         memcpy( &currentNoCompBlock[sizeOfCurrentNoCompBlock],&docids[start], copyLen*4 );
         sizeOfCurrentNoCompBlock = DEFAULT_BATCH_SIZE;
        
         //Add to the list of last element of each block
         baseListForOnlyCompBlocks.push_back(currentNoCompBlock[DEFAULT_BATCH_SIZE-1]);
         shared_ptr<CompressedDeltaChunk> compRes = PForDeltaCompressCurrentBlock();      
         sequenceOfCompBlocks.add(compRes);

         // the middle blocks (copy all possible full block)
         int leftLen = len - copyLen;
         int newStart = start + copyLen;
         while(leftLen > DEFAULT_BATCH_SIZE) {
            baseListForOnlyCompBlocks.push_back(docids[newStart+DEFAULT_BATCH_SIZE-1]);
            memcpy( &currentNoCompBlock[0],&docids[newStart], DEFAULT_BATCH_SIZE*4 );

            PForDeltaCompressCurrentBlock();
            compRes = PForDeltaCompressCurrentBlock();    
            sequenceOfCompBlocks.add(compRes);
            leftLen -= DEFAULT_BATCH_SIZE;
            newStart += DEFAULT_BATCH_SIZE;
         }

         // the last block
         if(leftLen > 0) {
            memcpy( &currentNoCompBlock[0],&docids[newStart], leftLen*4 );
         }
         sizeOfCurrentNoCompBlock = leftLen;
      }
      lastAdded = docids[start+len-1];
      totalDocIdNum += len;
    }

  /**
   * Add document to this set
   * Note that you must set the bits in increasing order:
   * addDoc(1), addDoc(2) is ok; 
   * addDoc(2), addDoc(1) is not ok.
   */
  void addDoc(unsigned int docId) {
    assert(docId != 0);  // Docid should start at 1
    if (PREDICT_TRUE(sizeOfCurrentNoCompBlock != DEFAULT_BATCH_SIZE)) { 
       currentNoCompBlock[sizeOfCurrentNoCompBlock++] = docId;
       lastAdded = docId;
    } else {
        //the last docId of the block      
        baseListForOnlyCompBlocks.push_back(lastAdded);

        // compress currentNoCompBlock[] (excluding the input docId),
        shared_ptr<CompressedDeltaChunk> compRes = PForDeltaCompressCurrentBlock();
        //compressedByteSize += (*compRes).getCompressedSize();      
        sequenceOfCompBlocks.add(compRes);

        // next block
        sizeOfCurrentNoCompBlock = 1;
        lastAdded = docId;
        currentNoCompBlock[0] = docId;  
    }
    totalDocIdNum++;
  }
  
  CompressedSet unorderedAdd(unsigned int docId){
	CompressedSet set;
	CompressedSet::Iterator it(this);
	bool inserted = false;
	while (it.nextDoc() != NO_MORE_DOCS ){
		unsigned int val = it.docID();
		if (val > docId && !inserted){
			inserted = true;
			set.addDoc(docId);
		}
		set.addDoc(val);	
	}
	if(!inserted){
		set.addDoc(docId);
	}
	return set;
  }

  CompressedSet removeDoc(unsigned int docId){
	CompressedSet set;
	CompressedSet::Iterator it(this);
	while (it.nextDoc() != NO_MORE_DOCS ){
		unsigned int val = it.docID();
		if (val != docId){
			set.addDoc(val);
		}	
	}
	return set;
  }

  void compactBaseListForOnlyCompBlocks(){
	if (baseListForOnlyCompBlocks.size() != baseListForOnlyCompBlocks.capacity()) {
        vector<unsigned int> tmp(baseListForOnlyCompBlocks);
        std::swap(baseListForOnlyCompBlocks, tmp);
    }

  }

  void compact(){
    sequenceOfCompBlocks.compact();
	compactBaseListForOnlyCompBlocks();
 }


  void initSet(){
    memset(currentNoCompBlock,0,DEFAULT_BATCH_SIZE);
  }

  /**
   *  Flush the data left in the currentNoCompBlock into the compressed data
   */
  void flush() {
    baseListForOnlyCompBlocks.push_back(currentNoCompBlock[sizeOfCurrentNoCompBlock-1]);
    preProcessBlock(currentNoCompBlock, sizeOfCurrentNoCompBlock);
    shared_ptr<CompressedDeltaChunk> compRes = PForDeltaCompressOneBlock(currentNoCompBlock,sizeOfCurrentNoCompBlock);
    
    //compressedByteSize += (*compRes).getCompressedSize();      
    sequenceOfCompBlocks.add(compRes);
    sizeOfCurrentNoCompBlock = 0;
  }

  /**
   * Prefix Sum
   * 
   */
  void preProcessBlock(unsigned int block[], size_t size)
  {
    for(int i=size-1; i>0; --i)
    {
      block[i] = block[i] - block[i-1] - 1;
    }
  }


  shared_ptr<CompressedDeltaChunk> PForDeltaCompressOneBlock(unsigned int* block,size_t blocksize){
    shared_ptr<CompressedDeltaChunk> compblock = codec.Compress(block,blocksize);
    return compblock;
  }

  shared_ptr<CompressedDeltaChunk> PForDeltaCompressCurrentBlock(){ 
    preProcessBlock(currentNoCompBlock, sizeOfCurrentNoCompBlock);
    shared_ptr<CompressedDeltaChunk> finalRes = PForDeltaCompressOneBlock(currentNoCompBlock,DEFAULT_BATCH_SIZE);
    return finalRes;  
  }

    /**
     * Gets the number of ids in the set
     * @return docset size
     */
    int size() const {
      return totalDocIdNum;
    }

    //This method will not work after a call to flush()
    inline bool find(unsigned int target) const { 
        //unsigned int lastId = lastAdded;
        if(PREDICT_FALSE(totalDocIdNum==0))
              return false;
        if (sizeOfCurrentNoCompBlock!=0){
            //int lastId = currentNoCompBlock[sizeOfCurrentNoCompBlock-1];
            if(target > lastAdded)
            {
              return false;
            }

            // first search noComp block
            if(baseListForOnlyCompBlocks.size()==0 || target>baseListForOnlyCompBlocks[baseListForOnlyCompBlocks.size()-1])
            {
              int i;
              for(i=0; i<sizeOfCurrentNoCompBlock; ++i)
              {
                if(currentNoCompBlock[i] >= target)
                  break;
              }
              if(i == sizeOfCurrentNoCompBlock) 
                return false;
              return currentNoCompBlock[i] == target; 
            }
        }

        // if we have some CompBlocks
        // first find which block to decompress by looking into baseListForOnlyCompBlocks
        if(baseListForOnlyCompBlocks.size()>0) {
            // baseListForOnlyCompBlocks.size() must then >0
           int iterDecompBlock = binarySearchInBaseListForBlockThatMayContainTarget(baseListForOnlyCompBlocks, 0, baseListForOnlyCompBlocks.size()-1, target);
           if(iterDecompBlock<0)
             return false; // target is bigger then biggest value
            
           ////uncompress block 
           Source src = sequenceOfCompBlocks.get(iterDecompBlock).getSource();
           size_t uncompSize = codec.Uncompress(src,myDecompBlock,DEFAULT_BATCH_SIZE);
           return codec.findInDeltaArray(myDecompBlock,uncompSize,target);
        }
        return false;
      }
};


   CompressedSet::Iterator::Iterator(const CompressedSet* const parentSet) {
        set = parentSet;
        compBlockNum = set->sequenceOfCompBlocks.size();

        lastAccessedDocId = -1;
        iterDecompBlock  = new unsigned int[DEFAULT_BATCH_SIZE];
        cursor = -1;
        totalDocIdNum = set->totalDocIdNum;
        if (set->totalDocIdNum <= 0){
            cursor = set->totalDocIdNum;
        }
    }



   //TODO have a copy constructor and assignator for iterator

   
   CompressedSet::Iterator::Iterator(const CompressedSet::Iterator& other){
    cursor = other.cursor;
    totalDocIdNum = other.totalDocIdNum;
    lastAccessedDocId = other.lastAccessedDocId;    
    compBlockNum = other.compBlockNum;

    iterDecompBlock  = new unsigned int[DEFAULT_BATCH_SIZE];
    memcpy(iterDecompBlock,other.iterDecompBlock,sizeof(unsigned int)*DEFAULT_BATCH_SIZE);

    set = other.set;
    //BLOCK_INDEX_SHIFT_BITS = other.BLOCK_INDEX_SHIFT_BITS;
   }

   CompressedSet::Iterator& CompressedSet::Iterator::operator=(const CompressedSet::Iterator& other){
    delete[] iterDecompBlock;
    cursor = other.cursor;
    totalDocIdNum = other.totalDocIdNum;
    lastAccessedDocId = other.lastAccessedDocId;    
    compBlockNum = other.compBlockNum;

    iterDecompBlock  = new unsigned int[DEFAULT_BATCH_SIZE];
    memcpy(iterDecompBlock,other.iterDecompBlock,sizeof(unsigned int)*DEFAULT_BATCH_SIZE);

    set = other.set;
    //BLOCK_INDEX_SHIFT_BITS = other.BLOCK_INDEX_SHIFT_BITS;
    return *this;
   }

   CompressedSet::Iterator::~Iterator(){
       delete[] iterDecompBlock;
   }

    /**
     * Get the index of the batch this cursor position falls into
     * 
     * @param index
     * @return
     */
    int CompressedSet::Iterator::getBlockIndex(int docIdIndex) {
      return docIdIndex >> BLOCK_INDEX_SHIFT_BITS;
    }

    int CompressedSet::Iterator::nextDoc(){
        //: if the pointer points to the end
        if(PREDICT_FALSE(++cursor == totalDocIdNum)) { 
          lastAccessedDocId = NO_MORE_DOCS;
        }else{
         int iterBlockIndex = cursor >> BLOCK_SIZE_BIT; 
         int offset = cursor & BLOCK_SIZE_MODULO;
         if( iterBlockIndex == compBlockNum  ) { 
             lastAccessedDocId = set->currentNoCompBlock[offset];
         } else
         if (PREDICT_TRUE(offset)){ 
	        //lastAccessedDocId = iterDecompBlock[offset];
             lastAccessedDocId += (iterDecompBlock[offset]+1);
         } else {
	        // (offset==0) must be in one of the compressed blocks
             Source src = set->sequenceOfCompBlocks.get(iterBlockIndex).getSource();
             size_t uncompSize = set->codec.Uncompress(src,iterDecompBlock,DEFAULT_BATCH_SIZE);
             lastAccessedDocId = iterDecompBlock[0];
         }
        }
        return lastAccessedDocId;
    }

    int CompressedSet::Iterator::docID(){
        return lastAccessedDocId;
    }


    /**
    * Implement the same functionality as advanceToTargetInTheFollowingCompBlocks() 
    * except that this function do prefix sum during searching
    *
    */
    int CompressedSet::Iterator::advanceToTargetInTheFollowingCompBlocksNoPostProcessing(int target, int startBlockIndex)
    {
      // searching from the current block
      int iterBlockIndex = binarySearchInBaseListForBlockThatMayContainTarget(set->baseListForOnlyCompBlocks, startBlockIndex, set->baseListForOnlyCompBlocks.size()-1, target);
      
      //"ERROR: advanceToTargetInTheFollowingCompBlocks(): Impossible, we must be able to find the block"
      assert(iterBlockIndex >= 0); 

      Source src = set->sequenceOfCompBlocks.get(iterBlockIndex).getSource();
      size_t uncompSize = set->codec.Uncompress(src,iterDecompBlock,DEFAULT_BATCH_SIZE);

      lastAccessedDocId = iterDecompBlock[0];
      if (lastAccessedDocId >= target) {
        cursor = (iterBlockIndex << BLOCK_INDEX_SHIFT_BITS) + 0;
        return lastAccessedDocId;
      }
   

      for (int offset=1; offset < uncompSize; ++offset)
      {
        lastAccessedDocId += ( iterDecompBlock[offset]+1);
        if (lastAccessedDocId >= target) {
          cursor = (iterBlockIndex << BLOCK_INDEX_SHIFT_BITS) + offset;
          return lastAccessedDocId;
        }
      }
      
      // "ERROR: case 2: advanceToTargetInTheFollowingCompBlocks(), Impossible, we must be able to find the target" + target + " in the block " + iterBlockIndex);
      assert(iterBlockIndex >= 0); 
      return -1;
    }

    // Advances to the first beyond the current 
    // whose value is greater than or equal to target.
    // we do linear search inside block because of delta encoding
    int CompressedSet::Iterator::Advance(int target){     
    // if the pointer points past the end
    if( PREDICT_FALSE(cursor == totalDocIdNum || totalDocIdNum <= 0)){
        lastAccessedDocId = NO_MORE_DOCS;
        return NO_MORE_DOCS;
    }
    // if the pointer points to the end
    if(++cursor == totalDocIdNum){
        lastAccessedDocId = NO_MORE_DOCS;
        return NO_MORE_DOCS;
    }
    // the expected behavior is to find the first element AFTER the current cursor, 
    // who is equal or larger than target
    if(target <= lastAccessedDocId) {
        target = lastAccessedDocId + 1;
    }
    
    int iterBlockIndex = cursor >> BLOCK_SIZE_BIT; 
    int offset = cursor & BLOCK_SIZE_MODULO;
    
    // if there is noComp block, check noComp block 
    // the next element is in currently in the last block , or
    // currently not in the last block, but the target is larger than
    // the last element of the last compressed block
    unsigned int sizeOfCurrentNoCompBlock = set->sizeOfCurrentNoCompBlock;
    size_t baseListForOnlyCompBlocksSize = set->baseListForOnlyCompBlocks.size();
    if(sizeOfCurrentNoCompBlock>0) {// if there exists the last decomp block 
      if(iterBlockIndex == compBlockNum || (baseListForOnlyCompBlocksSize>0 && target > set->baseListForOnlyCompBlocks[baseListForOnlyCompBlocksSize-1])) {   
        offset = binarySearchForFirstElementEqualOrLargerThanTarget(set->currentNoCompBlock, 0, sizeOfCurrentNoCompBlock-1, target);
   
        if(offset>=0){         
          iterBlockIndex = compBlockNum;
          lastAccessedDocId = set->currentNoCompBlock[offset];            
          cursor = (iterBlockIndex << BLOCK_INDEX_SHIFT_BITS) + offset;
          return lastAccessedDocId;
        } else {
          // hy: to avoid the repeated lookup next time once it reaches the end of the sequence
          cursor = totalDocIdNum; 
          lastAccessedDocId = NO_MORE_DOCS;
          return lastAccessedDocId;
        }
      }
    }
    
     // if we cannot not find it in the noComp block, we check the comp blocks
     if(baseListForOnlyCompBlocksSize>0 && target <= set->baseListForOnlyCompBlocks[baseListForOnlyCompBlocksSize-1]) {
       // for the following cases, it must exist in one of the comp blocks since target<= the last base in the comp blocks
       if(offset == 0) {
         // searching the right block from the current block to the last block
         lastAccessedDocId = advanceToTargetInTheFollowingCompBlocksNoPostProcessing(target, iterBlockIndex);
         return lastAccessedDocId;
       } else { // offset > 0, the current block has been decompressed, so, first test the first block; and then do sth like case 2       
         if(target <= set->baseListForOnlyCompBlocks[iterBlockIndex]) {
           while(offset < DEFAULT_BATCH_SIZE) {
             lastAccessedDocId += (iterDecompBlock[offset]+1);
            
             if (lastAccessedDocId >= target) {
               break;
             }
             offset++;
           }

           if (offset == DEFAULT_BATCH_SIZE) {
            printf("Error case 3: Impossible, we must be able to find the target %d in the block, lastAccessedDocId: %d , baseListForOnlyCompBlocks[%d]\n",
            target,lastAccessedDocId,iterBlockIndex);
           }
           assert(offset != DEFAULT_BATCH_SIZE);
       
           cursor = (iterBlockIndex << BLOCK_INDEX_SHIFT_BITS) + offset;    
           return lastAccessedDocId;
         } else { // hy: there must exist other comp blocks between the current block and noComp block since target <= baseListForOnlyCompBlocks.get(baseListForOnlyCompBlocks.size()-1)
           lastAccessedDocId = advanceToTargetInTheFollowingCompBlocksNoPostProcessing(target, iterBlockIndex);
           //lastAccessedDocId = LS_advanceToTargetInTheFollowingCompBlocks(target, iterBlockIndex);
           return lastAccessedDocId;
         }
       }
     }
    
     lastAccessedDocId = NO_MORE_DOCS;
     return lastAccessedDocId;
    }
#endif  // COMPRESSED_SET_H__
// DYNAMIC CACHING STRATEGY

// maintains a queue of all pages ordered by their recency information that are in
// main memory and pages that are not in main memory but were evicted recently.
// On a page fault, the system updates statistics on whether the access would have been a hit if 10%, 23%, 37%,
// or 50% of the memory was holding compressed pages
// Periodically, the compressed region
// size is changed to match the compressed region size that improves performance the most.

//compare at run-time an application’s performance on the
//compressed-memory system with an estimation of its performance without compression