#include <iostream>
#include <stdio.h>
#include <assert.h>
#include "Source.h"
#include "Sink.h"
#include <vector>
#include <memory>
#include "CollectionHelper.h"
#include "CompressedSet.h"
//baseListForOnlyCompBlocks
    Codec CompressedSet::codec;
    CompressedSet::CompressedSet(const CompressedSet& other)
		:  currentNoCompBlock(DEFAULT_BATCH_SIZE, 0)
	{
	    assert(totalDocIdNum <= 1); // You are trying to copy the bitmap, a terrible idea in general, for performance reasons
        //lastAdded = other.lastAdded;
        sizeOfCurrentNoCompBlock = other.sizeOfCurrentNoCompBlock;
        totalDocIdNum = other.totalDocIdNum;
        memcpy(&currentNoCompBlock[0], &other.currentNoCompBlock[0], sizeof(uint32_t)* DEFAULT_BATCH_SIZE);
    }


   /**
    * Swap the content of this bitmap with another bitmap.
    * No copying is done. (Running time complexity is constant.)
    */

    void CompressedSet::swap(CompressedSet & x){
	 //todo: implement it
     assert(false);
    }


 //   CompressedSet::CompressedSet() : currentNoCompBlock(DEFAULT_BATCH_SIZE, 0)
    CompressedSet::CompressedSet() : currentNoCompBlock(0)
	{
        //lastAdded = 0;
        sizeOfCurrentNoCompBlock = 0;
        totalDocIdNum = 0;
    }

    CompressedSet::~CompressedSet(){
    }


    void CompressedSet::write(ostream & out)
	{
        out.write((char*)&totalDocIdNum,4);
        if (totalDocIdNum > 0 ){
            out.write((char*)&sizeOfCurrentNoCompBlock,4);
	        out.write((char*)&currentNoCompBlock[0],sizeOfCurrentNoCompBlock*4);
        }


        if ( totalDocIdNum > DEFAULT_BATCH_SIZE) {
            //write base (skipping info)
            int baseListForOnlyCompBlocksSize = baseListForOnlyCompBlocks.size();
            out.write((char*)&baseListForOnlyCompBlocksSize,4);
            out.write((char*)&baseListForOnlyCompBlocks[0],baseListForOnlyCompBlocksSize*4);
            
            //write compressed blocks
            sequenceOfCompBlocks.write(out);
        }
        out.flush();
    }

    void CompressedSet::read(istream & in)
	{ 
		
        //read totalDocIdNum
        in.read((char*)&totalDocIdNum,4);
		if (totalDocIdNum>0)
		{
			in.read((char*)&sizeOfCurrentNoCompBlock,4);
            currentNoCompBlock.resize(sizeOfCurrentNoCompBlock);
            in.read((char*)&currentNoCompBlock[0],sizeOfCurrentNoCompBlock*4);

			if (totalDocIdNum  > DEFAULT_BATCH_SIZE){
		        //read base (skipping info)
		        int baseListForOnlyCompBlocksSize = 0;
		       	in.read((char*)&baseListForOnlyCompBlocksSize,4);
               	
		       	baseListForOnlyCompBlocks.clear();
		       	baseListForOnlyCompBlocks.resize(baseListForOnlyCompBlocksSize);
		       	in.read((char*)&baseListForOnlyCompBlocks[0],baseListForOnlyCompBlocksSize*4);
		       	//lastAdded = baseListForOnlyCompBlocks[baseListForOnlyCompBlocks.size()-1];
               	
               	
		       	//write compressed blocks
		       	sequenceOfCompBlocks.read(in);
			}
		} 
    }

    shared_ptr<Set::Iterator>  CompressedSet::iterator() const {
	   shared_ptr<Set::Iterator> it( new CompressedSet::Iterator(this));
       return it;
    }


    /**
     * Add an array of sorted docIds to the set
     */
    void CompressedSet::addDocs(unsigned int* docids,size_t start,size_t len){
      if ((len + sizeOfCurrentNoCompBlock) <= DEFAULT_BATCH_SIZE) {
	    currentNoCompBlock.resize(len + sizeOfCurrentNoCompBlock);
        memcpy( &currentNoCompBlock[sizeOfCurrentNoCompBlock],&docids[start], len*4 );
        sizeOfCurrentNoCompBlock += len;
      } else {
         // the first block can be completed so fillup a complet block
         int copyLen = DEFAULT_BATCH_SIZE - sizeOfCurrentNoCompBlock;
         currentNoCompBlock.resize(DEFAULT_BATCH_SIZE);
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
         //currentNoCompBlock.resize(sizeOfCurrentNoCompBlock);
      }
     // lastAdded = docids[start+len-1];

      totalDocIdNum += len;
    }

  /**
   * Add document to this set
   * Note that you must set the bits in increasing order:
   * addDoc(1), addDoc(2) is ok;
   * addDoc(2), addDoc(1) is not ok.
   */
  void CompressedSet::addDoc(unsigned int docId) {
    if (PREDICT_TRUE(sizeOfCurrentNoCompBlock != DEFAULT_BATCH_SIZE)) {
	   currentNoCompBlock.resize(sizeOfCurrentNoCompBlock+1);
       currentNoCompBlock[sizeOfCurrentNoCompBlock++] = docId;
      // lastAdded = docId;
    } else {
        //the last docId of the block
		baseListForOnlyCompBlocks.push_back(currentNoCompBlock[sizeOfCurrentNoCompBlock-1]);
       // baseListForOnlyCompBlocks.push_back(lastAdded);
     
        // compress currentNoCompBlock[] (excluding the input docId),
        shared_ptr<CompressedDeltaChunk> compRes = PForDeltaCompressCurrentBlock();
        sequenceOfCompBlocks.add(compRes);

        // next block
        sizeOfCurrentNoCompBlock = 1;
        currentNoCompBlock.resize(1);
        //lastAdded = docId;
        currentNoCompBlock[0] = docId;
    }
    totalDocIdNum++;
  }

  CompressedSet CompressedSet::unorderedAdd(unsigned int docId){
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

  CompressedSet CompressedSet::removeDoc(unsigned int docId){
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

  void CompressedSet::compactBaseListForOnlyCompBlocks(){
	if (baseListForOnlyCompBlocks.size() != baseListForOnlyCompBlocks.capacity()) {
        vector<unsigned int> tmp(baseListForOnlyCompBlocks);
        std::swap(baseListForOnlyCompBlocks, tmp);
    }

  }

 void CompressedSet::compact(){
    sequenceOfCompBlocks.compact();
	compactBaseListForOnlyCompBlocks();
 }






  /**
   * Prefix Sum
   *
   */
  void CompressedSet::preProcessBlock(unsigned int block[], size_t size)
  {
    for(int i=size-1; i>0; --i)
    {
      block[i] = block[i] - block[i-1] - 1;
    }
  }


  const shared_ptr<CompressedDeltaChunk> CompressedSet::PForDeltaCompressOneBlock(unsigned int* block,size_t blocksize){
    return codec.Compress(block,blocksize);
  }

  const shared_ptr<CompressedDeltaChunk> CompressedSet::PForDeltaCompressCurrentBlock(){
    preProcessBlock(&currentNoCompBlock[0], sizeOfCurrentNoCompBlock);
    const shared_ptr<CompressedDeltaChunk> finalRes = PForDeltaCompressOneBlock(&currentNoCompBlock[0], sizeOfCurrentNoCompBlock);
    return finalRes;
  }

    /**
     * Gets the number of ids in the set
     * @return docset size
     */
    unsigned int CompressedSet::size() const {
      return totalDocIdNum;
    }

    /**
     * if more then 1/8 of bit are set to 1 in range [minSetValue,maxSetvalue]
     * you should use EWAHBoolArray compression instead
     * because this compression will take at least 8 bits by positions
     */
    bool CompressedSet::isDense(){
		if (totalDocIdNum == 0) return false;
	  	CompressedSet set;
		CompressedSet::Iterator it(this);
		unsigned int minval = NO_MORE_DOCS;
		unsigned int maxval = 0;
		while (it.nextDoc() != NO_MORE_DOCS ){
			unsigned int val = it.docID();
			if (val < minval) minval = val;
			if (val > maxval) maxval = val;
		}
		int ratio =  (maxval-minval)/ (totalDocIdNum);
		return ratio > 8;
    }

    //This method will not work after a call to flush()
    inline bool CompressedSet::find(unsigned int target) const {

	    vector<uint32_t,AlignedSTLAllocator<uint32_t, 64>> myDecompBlock(DEFAULT_BATCH_SIZE, 0);
        //unsigned int lastId = lastAdded;
        if(PREDICT_FALSE(totalDocIdNum==0))
              return false;
        if (sizeOfCurrentNoCompBlock!=0){
            //int lastId = currentNoCompBlock[sizeOfCurrentNoCompBlock-1];
            if(sizeOfCurrentNoCompBlock > 0 && target > currentNoCompBlock[sizeOfCurrentNoCompBlock-1])
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
           int index = binarySearchInBaseListForBlockThatMayContainTarget(baseListForOnlyCompBlocks, 0, baseListForOnlyCompBlocks.size()-1, target);
           if(index<0)
             return false; // target is bigger then biggest value

           ////uncompress block
           Source src = sequenceOfCompBlocks.get(index).getSource();
           size_t uncompSize = codec.Uncompress(src, &myDecompBlock[0] ,DEFAULT_BATCH_SIZE);
           return codec.findInDeltaArray(&myDecompBlock[0], uncompSize,target);
        }
        return false;
    }


   CompressedSet::Iterator::Iterator(const CompressedSet* const parentSet) : iterDecompBlock(DEFAULT_BATCH_SIZE, 0)
   {
        set = parentSet;
        compBlockNum = set->sequenceOfCompBlocks.size();

        lastAccessedDocId = -1;
        // need to be fixed (iterDecompBlock should be 128bit aligned)
        // iterDecompBlock  = new unsigned int[DEFAULT_BATCH_SIZE];
        cursor = -1;
        totalDocIdNum = set->totalDocIdNum;
    }

   CompressedSet::Iterator::Iterator(const CompressedSet::Iterator& other) : iterDecompBlock(DEFAULT_BATCH_SIZE, 0)
   {
    cursor = other.cursor;
    totalDocIdNum = other.totalDocIdNum;
    lastAccessedDocId = other.lastAccessedDocId;
    compBlockNum = other.compBlockNum;
    // need to be fixed (iterDecompBlock should be 128bit aligned)
    // iterDecompBlock  = new unsigned int[DEFAULT_BATCH_SIZE];
    memcpy(&iterDecompBlock[0], &other.iterDecompBlock[0], sizeof(uint32_t)* DEFAULT_BATCH_SIZE);

    set = other.set;
    //BLOCK_INDEX_SHIFT_BITS = other.BLOCK_INDEX_SHIFT_BITS;
   }

   CompressedSet::Iterator& CompressedSet::Iterator::operator=(const CompressedSet::Iterator& other)
   {
    // delete[] iterDecompBlock;
    cursor = other.cursor;
    totalDocIdNum = other.totalDocIdNum;
    lastAccessedDocId = other.lastAccessedDocId;
    compBlockNum = other.compBlockNum;

    // iterDecompBlock  = new unsigned int[DEFAULT_BATCH_SIZE];
    memcpy(&iterDecompBlock[0], &other.iterDecompBlock[0], sizeof(uint32_t)* DEFAULT_BATCH_SIZE);

    set = other.set;
    //BLOCK_INDEX_SHIFT_BITS = other.BLOCK_INDEX_SHIFT_BITS;
    return *this;
   }

   CompressedSet::Iterator::~Iterator()
   {
       // delete[] iterDecompBlock;
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

    unsigned int CompressedSet::Iterator::nextDoc(){
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
             set->codec.Uncompress(src, &iterDecompBlock[0], DEFAULT_BATCH_SIZE);
            // assert(uncompSize == DEFAULT_BATCH_SIZE);
             lastAccessedDocId = iterDecompBlock[0];
         }
        }
        return lastAccessedDocId;
    }

    unsigned int CompressedSet::Iterator::docID(){
        return lastAccessedDocId;
    }


    /**
    * Implement the same functionality as advanceToTargetInTheFollowingCompBlocks()
    * except that this function do prefix sum during searching
    *
    */
    int CompressedSet::Iterator::advanceToTargetInTheFollowingCompBlocksNoPostProcessing(unsigned int target, int startBlockIndex)
    {
      // searching from the current block
      int iterBlockIndex = binarySearchInBaseListForBlockThatMayContainTarget(set->baseListForOnlyCompBlocks, startBlockIndex, set->baseListForOnlyCompBlocks.size()-1, target);

      //"ERROR: advanceToTargetInTheFollowingCompBlocks(): Impossible, we must be able to find the block"
      assert(iterBlockIndex >= 0);

      Source src = set->sequenceOfCompBlocks.get(iterBlockIndex).getSource();
      size_t uncompSize = set->codec.Uncompress(src, &iterDecompBlock[0], DEFAULT_BATCH_SIZE);

      lastAccessedDocId = iterDecompBlock[0];
      if (lastAccessedDocId >= target) {
        cursor = (iterBlockIndex << BLOCK_INDEX_SHIFT_BITS) + 0;
        return lastAccessedDocId;
      }


      for (size_t offset=1; offset < uncompSize; ++offset)
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
    unsigned int CompressedSet::Iterator::Advance(unsigned int target){
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
        offset = binarySearchForFirstElementEqualOrLargerThanTarget(&(set->currentNoCompBlock[0]), 0, sizeOfCurrentNoCompBlock-1, target);

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
