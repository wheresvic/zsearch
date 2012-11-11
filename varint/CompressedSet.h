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
#include "Common.h"
#include "Set.h"
#include "DeltaChunkStore.h"
#include "CompressedDeltaChunk.h"
#include "Codec.h"
#include "bitpacking/memutil.h"

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
        // unsigned int*  iterDecompBlock; // temporary storage for the decompressed data
		// unsigned int* currentNoCompBlock;
		vector<uint32_t,AlignedSTLAllocator<uint32_t, 64>> iterDecompBlock;
		vector<uint32_t,AlignedSTLAllocator<uint32_t, 64>> currentNoCompBlock;

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
    // Two separate arrays containing
    // the last docID
    // of each block in words in uncompressed form.
    vector<unsigned int> baseListForOnlyCompBlocks;


	const CompressedSet& operator=(const CompressedSet& other);


public:
	unsigned int lastAdded; // recently inserted/accessed element
    Codec codec; // varint encoding codec
    unsigned int totalDocIdNum; // the total number of elemnts that have been inserted/accessed so far
    // unsigned int* currentNoCompBlock;
    vector<uint32_t,AlignedSTLAllocator<uint32_t, 64>> currentNoCompBlock;  // the memory used to store the uncompressed elements. Once the block is full, all its elements are compressed into sequencOfCompBlock and the block is cleared.
    DeltaChunkStore sequenceOfCompBlocks; // Store for list compressed delta chunk


	CompressedSet(const CompressedSet& other);


    /**
     * Swap the content of this bitmap with another bitmap.
     * No copying is done. (Running time complexity is constant.)
     */
	void swap(CompressedSet & x);


    CompressedSet();

	~CompressedSet();

    /**
     *  Flush the data left in the currentNoCompBlock into the compressed data
     */
	void flush();

    void write(ostream & out);

	void read(istream & in);

	shared_ptr<Set::Iterator>  iterator() const;

    /**
     * Add an array of sorted docIds to the set
     */
	void addDocs(unsigned int docids[],size_t start,size_t len);

    /**
     * Add document to this set
     * Note that you must set the bits in increasing order:
     * addDoc(1), addDoc(2) is ok;
     * addDoc(2), addDoc(1) is not ok.
     */
	void addDoc(unsigned int docId);

    CompressedSet unorderedAdd(unsigned int docId);

    CompressedSet removeDoc(unsigned int docId);

    void compactBaseListForOnlyCompBlocks();

    void compact();

    void initSet();

    /**
     * Prefix Sum
     *
     */
    void preProcessBlock(unsigned int block[], size_t size);

    shared_ptr<CompressedDeltaChunk> PForDeltaCompressOneBlock(unsigned int* block,size_t blocksize);

	shared_ptr<CompressedDeltaChunk> PForDeltaCompressCurrentBlock();

    /**
     * Gets the number of ids in the set
     * @return docset size
     */
	int size() const;

    /**
     * if more then 1/8 of bit are set to 1 in range [minSetValue,maxSetvalue]
     * you should use EWAHBoolArray compression instead
     * because this compression will take at least 8 bits by positions
     */
	bool isDense();

    //This method will not work after a call to flush()
    bool find(unsigned int target) const;
};
#endif  // COMPRESSED_SET_H__
