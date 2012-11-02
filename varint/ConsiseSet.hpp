/**
 * This is CONCISE: COmpressed 'N' Composable Integer SET.
 * internally represented by compressed bitmaps 
 * though a RLE (Run-Length Encoding) compression algorithm.
 * see http://ricerca.mat.uniroma3.it/users/colanton/docs/concise.pdf for detail.
 */
#ifndef CONCISE_SET_H__
#define CONCISE_SET_H__
#include <vector>
class ConciseSet;

/**
 * Iterator over the bits of a single literal/fill word
 */
class WordExpander {
public:
	virtual bool hasNext() = 0;
	virtual bool hasPrevious() = 0;
	virtual int next()= 0;
	virtual int previous()= 0;
	virtual void skipAllAfter(int i)= 0;
	virtual void skipAllBefore(int i)= 0;
	virtual void reset(int offset, int word, boolean fromBeginning)= 0;
	virtual ~WordExpander(){}
};

/**
 * Iterator over the bits of literal and zero-fill words
 */
class LiteralAndZeroFillExpander : WordExpander {
	int[] buffer = new int[MAX_LITERAL_LENGHT];
	int len = 0;
	int current = 0;
public:
	bool hasNext() {
		return current < len;
	}
	
	bool hasPrevious() {
		return current > 0;
	}
	
	int next() {
		if (!hasNext())
			throw new NoSuchElementException();
		return buffer[current++];
	}
	
	int previous() {
		if (!hasPrevious())
			throw new NoSuchElementException();
		return buffer[--current];
	}
	
	void skipAllAfter(int i) {
		while (hasPrevious() && buffer[current - 1] > i)
			current--;
	}
	
	void skipAllBefore(int i) {
		while (hasNext() && buffer[current] < i)
			current++;
	}
	
	void reset(int offset, int word, boolean fromBeginning) {
		if (isLiteral(word)) {
			len = 0;
			for (int i = 0; i < MAX_LITERAL_LENGHT; i++) 
				if ((word & (1 << i)) != 0)
					buffer[len++] = offset + i;
			current = fromBeginning ? 0 : len;
		} else {
			if (isZeroSequence(word)) {
				if (simulateWAH || isSequenceWithNoBits(word)) {
					len = 0;
					current = 0;
				} else {
					len = 1;
					buffer[0] = offset + ((0x3FFFFFFF & word) >>> 25) - 1;
					current = fromBeginning ? 0 : 1;
				}
			} else {
				throw new RuntimeException("sequence of ones!");
			}
		}
	}
};
	
class BitIterator{
	WordExpander* exp;
	OneFillExpander oneExp = new OneFillExpander();
	LiteralAndZeroFillExpander litExp = new LiteralAndZeroFillExpander();	
	int nextIndex = 0;
	int nextOffset = 0;
	ConciseSet& set;
	
	BitIterator () {
		//nextWord();
	}	
	bool hasNext() {
		return nextIndex <= lastWordIndex || exp->hasNext();
	}
	
	void nextWord() {
		int word = set.words[nextIndex++];
		exp = set.isOneSequence(word) ? &oneExp : &litExp;
		exp->reset(nextOffset, word, true);
		
		// prepare next offset
		if (set.isLiteral(word)) {
			nextOffset += set.MAX_LITERAL_LENGHT;
		} else {
			nextOffset += set.maxLiteralLengthMultiplication(set.getSequenceCount(word) + 1);
		}
	}
	
	int next() {
		while (!exp->hasNext()) {
			if (nextIndex > set.lastWordIndex)
				throw new NoSuchElementException();
			nextWord();
		}
		return exp->next();
	}
	
	void skipAllBefore(int element) {
		while(true) {
			exp->skipAllBefore(element);
			if (exp->hasNext() || nextIndex > set.lastWordIndex)
				return;
			nextWord();
		}
	}
};
class ConciseSet {
	/**
	 * This is the compressed bitmap, that is a collection of words. 
	 * For each word:
	 *    1* (0x80000000) means that it is a 31-bit literal
     *
	 *    00* (0x00000000)  indicates a sequence made up of at 
	 *    most one set bit in the first 31 bits and followed by
	 *    blocks of 31 0's. The following 5 bits (00xxxxx*)
	 *    indicates which is the set bit 
	 *    ( 00000 = no set bit, 00001 = LSB, 11111 = MSB),
	 *    while the remaining 25 bits indicate the number of following 0's blocks.
	 *
	 *    01* (0x40000000) indicates a sequence made up of at most one unset bit in the first 31 bits, 
	 *    and followed by blocks of 31 1's. (see the 00* case above).
	 *  
	 * Note that literal words 0xFFFFFFFF and 0x80000000 are allowed, thus
	 * zero-length sequences (i.e., such that getSequenceCount() == 0) cannot exists.               
	 */
	vector<unsigned int> words;
	
	/**
	 * Most significant set bit within the uncompressed bit string.
	 */
	int last;

	/**
	 * Cached cardinality of the bit-set. Defined for efficient size()
	 * calls. When -1, the cache is invalid.
	 */	
	unsigned int size;
	
	/**
	 * Index of the last word in  #words
	 */
    int lastWordIndex;
	
	/**
	 * The highest representable integer.
	 */
	unsigned int  MAX_ALLOWED_INTEGER = 31 * (1 << 25) + 30; // 1040187422
	
	/** 
	 * The lowest representable integer.
	 */
	unsigned int MIN_ALLOWED_SET_BIT = 0;
	
	/** 
	 * Maximum number of representable bits within a literal
	 */
	unsigned MAX_LITERAL_LENGHT = 31;
	
	/**
	 * Literal that represents all bits set to 1 (and MSB = 1)
	 */
	unsigned int ALL_ONES_LITERAL = 0xFFFFFFFF;
	
	/**
	 * Literal that represents all bits set to 0 (and MSB = 1)
	 */
	unsigned int ALL_ZEROS_LITERAL = 0x80000000;
	
	/**
	 * All bits set to 1 and MSB = 0
	 */
	unsigned int ALL_ONES_WITHOUT_MSB = 0x7FFFFFFF;
	
	/**
	 * Sequence bit
	 */
	unsigned int SEQUENCE_BIT = 0x40000000;
	
public:	
	/**
	 * Creates an empty integer set
	 */
	ConciseSet() {

	}
	
	void reset() {
		//words = null;
		last = -1;
		size = 0;
		lastWordIndex = -1;
	}
	
	/**
	 * Calculates the modulus division by 31 in a faster way than using "n % 31"
	 *
	 * This method of finding modulus division by an integer that is one less
	 * than a power of 2 takes at most O(lg(32)) time.
	 * The number of operations is at most 12 + 9 * ceil(lg(32))
	 * see: http://graphics.stanford.edu/~seander/bithacks.html
	 */
	int maxLiteralLengthModulus(int n) {
		int m = (n & 0xC1F07C1F) + ((n >>> 5) & 0xC1F07C1F);
		m = (m >>> 15) + (m & 0x00007FFF);
		if (m <= 31)
			return m == 31 ? 0 : m;
		m = (m >>> 5) + (m & 0x0000001F);
		if (m <= 31)
			return m == 31 ? 0 : m;
		m = (m >>> 5) + (m & 0x0000001F);
		if (m <= 31)
			return m == 31 ? 0 : m;
		m = (m >>> 5) + (m & 0x0000001F);
		if (m <= 31)
			return m == 31 ? 0 : m;
		m = (m >>> 5) + (m & 0x0000001F);
		if (m <= 31)
			return m == 31 ? 0 : m;
		m = (m >>> 5) + (m & 0x0000001F);
		return m == 31 ? 0 : m;
	}
	
	/**
	 * Calculates the multiplication by 31 in a faster way than using <code>n * 31</code>
	 */
	int maxLiteralLengthMultiplication(int n) {
		return (n << 5) - n;
	}
	
	/**
	 * Calculates the division by 31
	 */
	int maxLiteralLengthDivision(int n) {
		return n / 31;
	}
	
	/**
	 * Checks whether a word is a literal one
	 */
	bool isLiteral(int word) {
		// "word" must be 1*
		// NOTE: this is faster than "return (word & 0x80000000) == 0x80000000"
		return (word & 0x80000000) != 0;
	}
	
	/**
	 * Checks whether a word contains a sequence of 1's
	 */
	 bool isOneSequence(int word) {
		// "word" must be 01*
		return (word & 0xC0000000) == SEQUENCE_BIT;
	 }
	
	 /**
	  * Checks whether a word contains a sequence of 0's
	  */
	 bool isZeroSequence(int word) {
		// "word" must be 00*
		return (word & 0xC0000000) == 0;
	 }
	
	 /**
	  * Checks whether a word contains a sequence of 0's with no set bit, or 1's
	  * with no unset bit.
	  */
	 bool isSequenceWithNoBits(int word) {
		// "word" must be 0?00000*
		return (word & 0xBE000000) == 0x00000000;
	 }
	
	 /**
	  * Gets the number of blocks of 1's or 0's stored in a sequence word
	  */
	 int getSequenceCount(int word) {
		// get the 25 LSB bits
		return word & 0x01FFFFFF;
	 }
	
	 /**
	  * Clears the (un)set bit in a sequence
	  */
	 int getSequenceWithNoBits(int word) {
		// clear 29 to 25 LSB bits
		return (word & 0xC1FFFFFF);
	 }
	
	 /**
	  * Gets the literal word that represents the first 31 bits of the given the
	  * word (i.e. the first block of a sequence word, or the bits of a literal word).
	  *
	  * If the word is a literal, it returns the unmodified word. In case of a
	  * sequence, it returns a literal that represents the first 31 bits of the
	  * given sequence word.
	  */
	 int getLiteral(int word) {
		if (isLiteral(word)) 
			return word;

		// get bits from 30 to 26 and use them to set the corresponding bit
		// NOTE: "1 << (word >>> 25)" and "1 << ((word >>> 25) & 0x0000001F)" are equivalent
		// NOTE: ">>> 1" is required since 00000 represents no bits and 00001 the LSB bit set
		int literal = (1 << (word >>> 25)) >>> 1;  
		return isZeroSequence(word) 
				? (ALL_ZEROS_LITERAL | literal) 
				: (ALL_ONES_LITERAL & ~literal);
	 }
	
	/**
	 * Gets the position of the flipped bit within a sequence word. If the
	 * sequence has no set/unset bit, returns -1.
	 * 
	 * Note that the parameter *must* a sequence word, otherwise the
	 * result is meaningless.
	 */
	int getFlippedBit(int word) {
		// get bits from 30 to 26
		// NOTE: "-1" is required since 00000 represents no bits and 00001 the LSB bit set
		return ((word >>> 25) & 0x0000001F) - 1;  
	}
	
	/**
	 * Gets the number of set bits within the literal word
	 */
	int getLiteralBitCount(int word) {
		return BitCount.count(getLiteralBits(word));
	}
	
	/**
	 * Gets the bits contained within the literal word
	 */
	int getLiteralBits(int word) {
		return ALL_ONES_WITHOUT_MSB & word;
	}
	
	/**
	 * Clears bits from MSB (excluded, since it indicates the word type) to the
	 * specified bit (excluded). Last word is supposed to be a literal one.
	 */
	clearBitsAfterInLastWord(int lastSetBit) {
		words[lastWordIndex] &= ALL_ZEROS_LITERAL | (0xFFFFFFFF >>> (31 - lastSetBit));
	}
	
	/**
	 * Returns <code>true</code> when the given 31-bit literal string (namely,
	 * with MSB set) contains only one set bit
	 */
	bool containsOnlyOneBit(int literal) {
		return (literal & (literal - 1)) == 0;
	}
	
	/**
	 * Assures that the length of {@link #words} is sufficient to contain
	 * the given index.
	 */
	void ensureCapacity(int index) {
		//TODO: words.resize(index);
	}
	
	/**
	 * Removes unused allocated words at the end of {@link #words} only when they
	 * are more than twice of the needed space
	 */
	private void compact() {
		// TODO: compact words
	}
};
#endif  // COMPRESSED_SET_H__