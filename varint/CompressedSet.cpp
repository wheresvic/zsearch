#include <iostream>
#include <stdio.h>
#include <assert.h>
#include "Source.h"
#include "Sink.h"
#include <vector>
#include <memory>
#include "CollectionHelper.h"
#include "CompressedSet.h"

    Codec CompressedSet::codec;
    CompressedSet::CompressedSet(const CompressedSet& other)
        :  currentNoCompBlock(DEFAULT_BATCH_SIZE, 0)
    {
        assert(totalDocIdNum <= 1);// You are trying to copy the bitmap, a terrible idea in general, for performance reasons
        sizeOfCurrentNoCompBlock = other.sizeOfCurrentNoCompBlock;
        totalDocIdNum = other.totalDocIdNum;
        memcpy(&currentNoCompBlock[0], &other.currentNoCompBlock[0], sizeof(uint32_t)* DEFAULT_BATCH_SIZE);
    }

    // CompressedSet::CompressedSet() : currentNoCompBlock(DEFAULT_BATCH_SIZE, 0)
    CompressedSet::CompressedSet() : currentNoCompBlock(0)
    {
        sizeOfCurrentNoCompBlock = 0;
        totalDocIdNum = 0;
    }

    CompressedSet::~CompressedSet(){
    }


    void CompressedSet::write(ostream& out)
    {
        out.write((char*)&totalDocIdNum,4);
        if (totalDocIdNum > 0 ){
            if (totalDocIdNum >= DEFAULT_BATCH_SIZE) {
               out.write((char*)&sizeOfCurrentNoCompBlock,4);
            }
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

    void CompressedSet::read(istream& in)
    {
        //read totalDocIdNum
        in.read((char*)&totalDocIdNum,4);
        if (totalDocIdNum>0) {
            if (totalDocIdNum < DEFAULT_BATCH_SIZE) {
                sizeOfCurrentNoCompBlock = totalDocIdNum;
            } else {
                in.read((char*)&sizeOfCurrentNoCompBlock,4); 
            }

            currentNoCompBlock.resize(sizeOfCurrentNoCompBlock);
            in.read((char*)&currentNoCompBlock[0],sizeOfCurrentNoCompBlock*4);

            if (totalDocIdNum  > DEFAULT_BATCH_SIZE) {
                //read base (skipping info)
                int baseListForOnlyCompBlocksSize = 0;
                   in.read((char*)&baseListForOnlyCompBlocksSize,4);

                   baseListForOnlyCompBlocks.clear();
                   baseListForOnlyCompBlocks.resize(baseListForOnlyCompBlocksSize);
                   in.read((char*)&baseListForOnlyCompBlocks[0],baseListForOnlyCompBlocksSize*4);

                   //write compressed blocks
                   sequenceOfCompBlocks.read(in);
            }
        }
    }

    shared_ptr<Set::Iterator> CompressedSet::iterator() const {
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
    } else {
        //the last docId of the block
        baseListForOnlyCompBlocks.push_back(currentNoCompBlock[sizeOfCurrentNoCompBlock-1]);

        // compress currentNoCompBlock[] (excluding the input docId),
        shared_ptr<CompressedDeltaChunk> compRes = PForDeltaCompressCurrentBlock();
        sequenceOfCompBlocks.add(compRes);

        // next block
        sizeOfCurrentNoCompBlock = 1;
        currentNoCompBlock.resize(1);
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

    void CompressedSet::removeDocId(unsigned int docId)
    {
        // TODO:

        /*
        CompressedSet set;
        set = this->removeDoc(docId);
        this->swap(set);
        */

        // or fix the interface to be able to return Set instead of CompressedSet

        throw -17;
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
   /*
  static void delta(unsigned int block[], size_t size) {
    for(int i=size-1;i>0;--i)
    {
      block[i] = block[i] - block[i-1];
    }
  }

  static void inverseDelta(unsigned int data[], const size_t size) {
        if (size == 0)
            return;
        size_t i = 1;
        for (;i < size - 1;i += 2) {
            data[i] += data[i - 1];
            data[i + 1] += data[i];
        }
        for (;i != size;++i) {
            data[i] += data[i - 1];
        }
  }

  // Simd delta
  void CompressedSet::preProcessBlock(unsigned int pData[], size_t TotalQty){
     if (TotalQty < 5) {
         delta(pData, TotalQty);// no SIMD
         return;
     }

     const size_t Qty4 = TotalQty / 4;
     __m128i* pCurr = reinterpret_cast<__m128i*>(pData);
     const __m128i* pEnd = pCurr + Qty4;

     __m128i last = _mm_setzero_si128();
     while (pCurr < pEnd) {
         __m128i a0 = _mm_load_si128(pCurr);
         __m128i a1 = _mm_sub_epi32(a0, _mm_srli_si128(last, 12));
         a1 = _mm_sub_epi32(a1, _mm_slli_si128(a0, 4));
         last = a0;
        
         _mm_store_si128(pCurr++ , a1);
     }

     if (Qty4 * 4 < TotalQty) {
         uint32_t lastVal = _mm_cvtsi128_si32(_mm_srli_si128(last, 12));
         for (size_t i = Qty4 * 4;i < TotalQty;++i) {
             uint32_t newVal = pData[i];
             pData[i] -= lastVal;
             lastVal = newVal;
         }
     }
  }

  // simd inverseDelta  
  static void postProcessBlock(unsigned int* pData, const size_t TotalQty) {
     if (TotalQty < 5) {
         inverseDelta(pData, TotalQty);// no SIMD
         return;
     }
     const size_t Qty4 = TotalQty / 4;

     __m128i runningCount = _mm_setzero_si128();
     __m128i* pCurr = reinterpret_cast<__m128i*>(pData);
     const __m128i* pEnd = pCurr + Qty4;
     while (pCurr < pEnd) {
         __m128i a0 = _mm_load_si128(pCurr);
         __m128i a1 = _mm_add_epi32(_mm_slli_si128(a0, 8),a0);
         __m128i a2 = _mm_add_epi32(_mm_slli_si128(a1, 4),a1);
         a0 = _mm_add_epi32(a2,runningCount);
         runningCount = _mm_shuffle_epi32(a0, 0xFF);
         _mm_store_si128(pCurr++ , a0);
     }

     for (size_t i = Qty4 * 4;i < TotalQty;++i) {
         pData[i] += pData[i-1];
     }
  }
  */
  
  const shared_ptr<CompressedDeltaChunk> CompressedSet::PForDeltaCompressOneBlock(unsigned int* block,size_t blocksize){
    return codec.Compress(block,blocksize);
  }

  const shared_ptr<CompressedDeltaChunk> CompressedSet::PForDeltaCompressCurrentBlock(){
    // preProcessBlock not needed when using integrated deltacoding
    // preProcessBlock(&currentNoCompBlock[0], sizeOfCurrentNoCompBlock);
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
              for(i=0;i<sizeOfCurrentNoCompBlock;++i)
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
             return false;// target is bigger then biggest value

           ////uncompress block
           Source src = sequenceOfCompBlocks.get(index).getSource();
           size_t uncompSize = codec.Uncompress(src, &myDecompBlock[0] ,DEFAULT_BATCH_SIZE);
           return codec.findInArray(&myDecompBlock[0], uncompSize,target);
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
        } else {
             int iterBlockIndex = cursor >> BLOCK_SIZE_BIT;
             int offset = cursor & BLOCK_SIZE_MODULO;
             if( iterBlockIndex == compBlockNum  ) {
                 lastAccessedDocId = set->currentNoCompBlock[offset];
             } else { 
                 if (PREDICT_TRUE(offset)){
                    //lastAccessedDocId = iterDecompBlock[offset];
                    #ifdef PREFIX_SUM
                       lastAccessedDocId += (iterDecompBlock[offset]);
                    #else
                       lastAccessedDocId = iterDecompBlock[offset];
                    #endif  
                 } else {
                    // (offset==0) must be in one of the compressed blocks
                    Source src = set->sequenceOfCompBlocks.get(iterBlockIndex).getSource();
                    set->codec.Uncompress(src, &iterDecompBlock[0], DEFAULT_BATCH_SIZE);
                    #ifndef PREFIX_SUM
                      // postProcessBlock not needed if using integrated delta coding
                      // postProcessBlock(&iterDecompBlock[0], DEFAULT_BATCH_SIZE);
                    #endif       
                    // assert(uncompSize == DEFAULT_BATCH_SIZE);
                     lastAccessedDocId = iterDecompBlock[0];
                 }
             }
        }
        return lastAccessedDocId;
    }

    unsigned int CompressedSet::Iterator::docID(){
        return lastAccessedDocId;
    }

    int CompressedSet::Iterator::advanceToTargetInTheFollowingCompBlocks(int target, int startBlockIndex)
    {
      // searching from the current block
      int iterBlockIndex = binarySearchInBaseListForBlockThatMayContainTarget(set->baseListForOnlyCompBlocks, startBlockIndex, set->baseListForOnlyCompBlocks.size()-1, target);
      assert(iterBlockIndex >= 0);
      if(iterBlockIndex < 0)
      {
        //System.err.println("ERROR: advanceToTargetInTheFollowingCompBlocks(): Impossible, we must be able to find the block");
      }
      int currentBlockIndex = cursor >> BLOCK_SIZE_BIT;
      int currentOffset = cursor & BLOCK_SIZE_MODULO;

      // if we are already on currentBlockIndex and currentOffset >0 
      // this mean the block is already decompresed by last iteration
      // we can skip this decompressing it again
      if (currentBlockIndex != iterBlockIndex || currentOffset == 0){
          Source src = set->sequenceOfCompBlocks.get(iterBlockIndex).getSource();
          set->codec.Uncompress(src, &iterDecompBlock[0], DEFAULT_BATCH_SIZE);
          //postProcessBlock not needed if using integrated delta coding
          //postProcessBlock(&iterDecompBlock[0], DEFAULT_BATCH_SIZE);
      }
      
      int offset = binarySearchForFirstElementEqualOrLargerThanTarget(&(iterDecompBlock[0]), 0, DEFAULT_BATCH_SIZE-1, target);
      assert(offset >= 0);
      if(offset < 0)
      {
       // System.err.println("ERROR: case 2: advanceToTargetInTheFollowingCompBlocks(), Impossible, we must be able to find the target" + target + " in the block " + iterBlockIndex);
      }

      cursor = (iterBlockIndex << BLOCK_INDEX_SHIFT_BITS) + offset;
      return iterDecompBlock[offset];
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

      /*
        iterBlockIndex might be the current uncompressed block !!!
      */
      
      //"ERROR: advanceToTargetInTheFollowingCompBlocks(): Impossible, we must be able to find the block"
      assert(iterBlockIndex >= 0);

      Source src = set->sequenceOfCompBlocks.get(iterBlockIndex).getSource();
      size_t uncompSize = set->codec.Uncompress(src, &iterDecompBlock[0], DEFAULT_BATCH_SIZE);

      lastAccessedDocId = iterDecompBlock[0];
      if (lastAccessedDocId >= target) {
        cursor = (iterBlockIndex << BLOCK_INDEX_SHIFT_BITS) + 0;
        return lastAccessedDocId;
      }
      
      // int currentoffset = cursor & BLOCK_SIZE_MODULO;
      for (size_t offset=1;offset < uncompSize;++offset)
      {
        lastAccessedDocId += ( iterDecompBlock[offset]);
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
        if(cursor != 0 && target <= lastAccessedDocId) {
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
          if(iterBlockIndex == compBlockNum ||
             (baseListForOnlyCompBlocksSize>0 && target > set->baseListForOnlyCompBlocks[baseListForOnlyCompBlocksSize-1])) {
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
            #ifdef PREFIX_SUM
                lastAccessedDocId = advanceToTargetInTheFollowingCompBlocksNoPostProcessing(target, iterBlockIndex);
            #else
                lastAccessedDocId = advanceToTargetInTheFollowingCompBlocks(target, iterBlockIndex);
            #endif
             return lastAccessedDocId;
           } else { // offset > 0, the current block has been decompressed, so, first test the first block;and then do sth like case 2
             assert(offset > 0);
             if(target <= set->baseListForOnlyCompBlocks[iterBlockIndex]) {
               while(offset < DEFAULT_BATCH_SIZE) {
                 #ifdef PREFIX_SUM
                    lastAccessedDocId += (iterDecompBlock[offset]);
                 #else
                    lastAccessedDocId = (iterDecompBlock[offset]);
                 #endif  
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
                #ifdef PREFIX_SUM
                   lastAccessedDocId = advanceToTargetInTheFollowingCompBlocksNoPostProcessing(target, iterBlockIndex);
                #else
                   lastAccessedDocId = advanceToTargetInTheFollowingCompBlocks(target, iterBlockIndex);
                #endif  
                return lastAccessedDocId;
             }
           }
         }

         lastAccessedDocId = NO_MORE_DOCS;
         return lastAccessedDocId;
    }
