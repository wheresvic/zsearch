/**
 * This is CONCISE: COmpressed 'N' Composable Integer SET.
 * internally represented by compressed bitmaps 
 * though a RLE (Run-Length Encoding) compression algorithm.
 * see http://ricerca.mat.uniroma3.it/users/colanton/docs/concise.pdf for detail.
 */
#ifndef CONCISE_SET_H__
#define CONCISE_SET_H__
#include <vector>

class ConciseSet {
public:
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
	 * User for <i>fail-fast</i> iterator. It counts the number of operations
	 * that <i>do</i> modify words
	 */
	int modCount = 0;
	
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
	unsigned int MAX_LITERAL_LENGHT = 31;
	
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

        bool simulateWAH = false;

	ConciseSet();
        //ConciseSet* clone()
        void reset();
	int maxLiteralLengthModulus(unsigned int n);
	int maxLiteralLengthMultiplication(int n);
	int maxLiteralLengthDivision(int n);
	bool isLiteral(int word);
	bool isOneSequence(int word);
	bool isZeroSequence(int word);
	bool isSequenceWithNoBits(int word);
	int getSequenceCount(int word);
	int getSequenceWithNoBits(int word);
	int getLiteral(unsigned int word);
	int getFlippedBit(unsigned int word);
	int getLiteralBitCount(int word);
	int getLiteralBits(int word);
	void clearBitsAfterInLastWord(unsigned int lastSetBit);
	bool containsOnlyOneBit(int literal);
	void ensureCapacity(int index);
	void compact();
	bool add(int e);
	void append(int i);
        void appendLiteral(int word);
	void appendFill(int length, int fillType);
        void updateLast();
 	bool isEmpty() {
		return words.size() == 0;
	}
};

class NoSuchElementException {};
class IndexOutOfBoundsException{
public:
	IndexOutOfBoundsException(int index){}
};

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
	virtual void reset(int offset, int word, bool fromBeginning)= 0;
	virtual ~WordExpander(){}
};

/**
 * Iterator over the bits of literal and zero-fill words
 */
class LiteralAndZeroFillExpander : WordExpander {
	int* buffer;
	int len = 0;
	int current = 0;
	ConciseSet& set;
public:
	LiteralAndZeroFillExpander(ConciseSet& set): set(set){
		buffer = new int[set.MAX_LITERAL_LENGHT];
	}
	~LiteralAndZeroFillExpander(){
		delete buffer;
	}
	bool hasNext() {
		return current < len;
	}
	
	bool hasPrevious() {
		return current > 0;
	}
	
	int next() {
		if (!hasNext())
			throw NoSuchElementException();
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
	
	void reset(int offset, int word, bool fromBeginning) {
		if (set.isLiteral(word)) {
			len = 0;
			for (int i = 0; i < set.MAX_LITERAL_LENGHT; i++) 
				if ((word & (1 << i)) != 0)
					buffer[len++] = offset + i;
			current = fromBeginning ? 0 : len;
		} else {
			if (set.isZeroSequence(word)) {
				if (set.simulateWAH || set.isSequenceWithNoBits(word)) {
					len = 0;
					current = 0;
				} else {
					len = 1;
					unsigned int uword =word;
					buffer[0] = offset + ((0x3FFFFFFF & uword) >> 25) - 1;
					current = fromBeginning ? 0 : 1;
				}
			} else {
				throw string("sequence of ones!");
			}
		}
	}
};

/**
 * Iterator over the bits of one-fill words
 */
class OneFillExpander : WordExpander {
	int firstInt = 1;
	int lastInt = -1;
	int current = 0;
	int exception = -1;
	ConciseSet& set;
public:
		OneFillExpander(ConciseSet& set): set(set){
			
		}
		~OneFillExpander(){
		}	
		bool hasNext() {
			return current < lastInt;
		}
		virtual bool hasPrevious() {
			return current > firstInt;
		}
		virtual int next(){
			if (!hasNext())
				throw new NoSuchElementException();
			current++;
			if (!set.simulateWAH && current == exception)
				current++;
			return current;
		}
		virtual int previous(){
			if (!hasPrevious())
				throw new NoSuchElementException();
			current--;
			if (!set.simulateWAH && current == exception)
				current--;
			return current;
		}
		virtual void skipAllAfter(int i){
			if (i >= current)
				return;
			current = i + 1;
		}
		virtual void skipAllBefore(int i){
			if (i <= current)
				return;
			current = i - 1;
		}
		virtual void reset(int offset, int word, bool fromBeginning){
			if (!set.isOneSequence(word))
				throw "NOT a sequence of ones!";
			firstInt = offset;
			lastInt = offset + set.maxLiteralLengthMultiplication(set.getSequenceCount(word) + 1) - 1;
			if (!set.simulateWAH) {
				unsigned int uword=word;
				exception = offset + ((0x3FFFFFFF & uword) >> 25) - 1;
				if (exception == firstInt)
					firstInt++;
				if (exception == lastInt)
					lastInt--;
			}
			current = fromBeginning ? (firstInt - 1) : (lastInt + 1);	
		}
};
	
class BitIterator {
	ConciseSet& set;
	WordExpander* exp;
	OneFillExpander oneExp;
	LiteralAndZeroFillExpander litExp;
	int nextIndex = 0;
	int nextOffset = 0;

	
	BitIterator (ConciseSet& set) : set(set),oneExp(set),litExp(set)  {
		//nextWord();
	}	
	bool hasNext() {
		return nextIndex <= set.lastWordIndex || exp->hasNext();
	}
	
	void nextWord() {
		int word = set.words[nextIndex++];
		exp = (set.isOneSequence(word) ? (WordExpander*)&oneExp : (WordExpander*)&litExp);
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

	/**
	 * Creates an empty integer set
	 */
	ConciseSet::ConciseSet() {

	}
	
	void ConciseSet::reset() {
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
	int ConciseSet::maxLiteralLengthModulus(unsigned int n) {
		unsigned int m = (n & 0xC1F07C1F) + ((n >> 5) & 0xC1F07C1F);
		m = (m >> 15) + (m & 0x00007FFF);
		if (m <= 31)
			return m == 31 ? 0 : m;
		m = (m >> 5) + (m & 0x0000001F);
		if (m <= 31)
			return m == 31 ? 0 : m;
		m = (m >> 5) + (m & 0x0000001F);
		if (m <= 31)
			return m == 31 ? 0 : m;
		m = (m >> 5) + (m & 0x0000001F);
		if (m <= 31)
			return m == 31 ? 0 : m;
		m = (m >> 5) + (m & 0x0000001F);
		if (m <= 31)
			return m == 31 ? 0 : m;
		m = (m >> 5) + (m & 0x0000001F);
		return m == 31 ? 0 : m;
	}
	
	/**
	 * Calculates the multiplication by 31 in a faster way than using <code>n * 31</code>
	 */
	int ConciseSet::maxLiteralLengthMultiplication(int n) {
		return (n << 5) - n;
	}
	
	/**
	 * Calculates the division by 31
	 */
	int ConciseSet::maxLiteralLengthDivision(int n) {
		return n / 31;
	}
	
	/**
	 * Checks whether a word is a literal one
	 */
	bool ConciseSet::isLiteral(int word) {
		// "word" must be 1*
		// NOTE: this is faster than "return (word & 0x80000000) == 0x80000000"
		return (word & 0x80000000) != 0;
	}
	
	/**
	 * Checks whether a word contains a sequence of 1's
	 */
	 bool ConciseSet::isOneSequence(int word) {
		// "word" must be 01*
		return (word & 0xC0000000) == SEQUENCE_BIT;
	 }
	
	 /**
	  * Checks whether a word contains a sequence of 0's
	  */
	 bool ConciseSet::isZeroSequence(int word) {
		// "word" must be 00*
		return (word & 0xC0000000) == 0;
	 }
	
	 /**
	  * Checks whether a word contains a sequence of 0's with no set bit, or 1's
	  * with no unset bit.
	  */
	 bool ConciseSet::isSequenceWithNoBits(int word) {
		// "word" must be 0?00000*
		return (word & 0xBE000000) == 0x00000000;
	 }
	
	 /**
	  * Gets the number of blocks of 1's or 0's stored in a sequence word
	  */
	 int ConciseSet::getSequenceCount(int word) {
		// get the 25 LSB bits
		return word & 0x01FFFFFF;
	 }
	
	 /**
	  * Clears the (un)set bit in a sequence
	  */
	 int ConciseSet::getSequenceWithNoBits(int word) {
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
	 int ConciseSet::getLiteral(unsigned int word) {
		if (isLiteral(word)) 
			return word;

		// get bits from 30 to 26 and use them to set the corresponding bit
		// NOTE: "1 << (word >> 25)" and "1 << ((word >>> 25) & 0x0000001F)" are equivalent
		// NOTE: ">>> 1" is required since 00000 represents no bits and 00001 the LSB bit set
		int literal = (1 << (word >> 25)) >> 1;  
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
	int ConciseSet::getFlippedBit(unsigned int word) {
		// get bits from 30 to 26
		// NOTE: "-1" is required since 00000 represents no bits and 00001 the LSB bit set
		return ((word >> 25) & 0x0000001F) - 1;  
	}
	
	/**
	 * Population count
	 */
	static int bitcount(unsigned int word) {
		word -= ((word >> 1) & 0x55555555);
		word = (word & 0x33333333) + ((word >> 2) & 0x33333333);
		word = (word + (word >> 4)) & 0x0F0F0F0F;
		return (word * 0x01010101) >> 24;
	}
	
	/**
	 * Gets the number of set bits within the literal word
	 */
	int ConciseSet::getLiteralBitCount(int word) {
		return bitcount(getLiteralBits(word));
	}
	
	/**
	 * Gets the bits contained within the literal word
	 */
	int ConciseSet::getLiteralBits(int word) {
		return ALL_ONES_WITHOUT_MSB & word;
	}
	
	/**
	 * Clears bits from MSB (excluded, since it indicates the word type) to the
	 * specified bit (excluded). Last word is supposed to be a literal one.
	 */
	void ConciseSet::clearBitsAfterInLastWord(unsigned int lastSetBit) {
		words[lastWordIndex] &= ALL_ZEROS_LITERAL | (0xFFFFFFFF >> (31 - lastSetBit));
	}
	
	/**
	 * Returns <code>true</code> when the given 31-bit literal string (namely,
	 * with MSB set) contains only one set bit
	 */
	bool ConciseSet::containsOnlyOneBit(int literal) {
		return (literal & (literal - 1)) == 0;
	}
	
	/**
	 * Assures that the length of {@link #words} is sufficient to contain
	 * the given index.
	 */
	void ConciseSet::ensureCapacity(int index) {
		//TODO: words.resize(index);
	}
	
	/**
	 * Removes unused allocated words at the end of {@link #words} only when they
	 * are more than twice of the needed space
	 */
        void ConciseSet::compact() {
		// TODO: compact words
	}
	
        bool ConciseSet::add(int e) {
		modCount++;

		// range check
		if (e < MIN_ALLOWED_SET_BIT || e > MAX_ALLOWED_INTEGER)
			throw  IndexOutOfBoundsException(e);

		// the element can be simply appended
		if (e > last) {
			append(e);
			return true;
		}

		if (e == last)
			return false;

		// check if the element can be put in a literal word
		int blockIndex = maxLiteralLengthDivision(e);
		int bitPosition = maxLiteralLengthModulus(e);
		for (int i = 0; i <= lastWordIndex && blockIndex >= 0; i++) {
			int w = words[i];
			if (isLiteral(w)) {
				// check if the current literal word is the "right" one
				if (blockIndex == 0) {
					// bit already set
					if ((w & (1 << bitPosition)) != 0)
						return false;
					
					// By adding the bit we potentially create a sequence:
					// -- If the literal is made up of all zeros, it definitely
					//    cannot be part of a sequence (otherwise it would not have
					//    been created). Thus, we can create a 1-bit literal word
					// -- If there are MAX_LITERAL_LENGHT - 2 set bits, by adding 
					//    the new one we potentially allow for a 1's sequence 
					//    together with the successive word
					// -- If there are MAX_LITERAL_LENGHT - 1 set bits, by adding 
					//    the new one we potentially allow for a 1's sequence 
					//    together with the successive and/or the preceding words
					if (!simulateWAH) {
						int bitCount = getLiteralBitCount(w);
						if (bitCount >= MAX_LITERAL_LENGHT - 2)
							break;
					} else {
						if (containsOnlyOneBit(~w) || w == ALL_ONES_LITERAL)
							break;
					}
						
					// set the bit
					words[i] |= 1 << bitPosition;
					if (size >= 0)
						size++;
					return true;
				} 
				
				blockIndex--;
			} else {
				if (simulateWAH) {
					if (isOneSequence(w) && blockIndex <= getSequenceCount(w))
						return false;
				} else {
					// if we are at the beginning of a sequence, and it is
					// a set bit, the bit already exists
					if (blockIndex == 0 
							&& (getLiteral(w) & (1 << bitPosition)) != 0)
						return false;
					
					// if we are in the middle of a sequence of 1's, the bit already exist
					if (blockIndex > 0 
							&& blockIndex <= getSequenceCount(w) 
							&& isOneSequence(w))
						return false;
				}

				// next word
				blockIndex -= getSequenceCount(w) + 1;
			}
		}
		throw "not handled yet";
		// the bit is in the middle of a sequence or it may cause a literal to
		// become a sequence, thus the "easiest" way to add it is by ORing
		//return replaceWith(performOperation(convert(e), Operator.OR));
	}
	
	void ConciseSet::append(int i) {
		// special case of empty set
		if (isEmpty()) {
			int zeroBlocks = maxLiteralLengthDivision(i);
			if (zeroBlocks == 0) {
				words.resize(1);
				lastWordIndex = 0;
			} else if (zeroBlocks == 1) {
				words.resize(2);
				lastWordIndex = 1;
				words[0] = ALL_ZEROS_LITERAL;
			} else {
				words.resize(2);
				lastWordIndex = 1;
				words[0] = zeroBlocks - 1;
			}
			last = i;
			size = 1;
			words[lastWordIndex] = ALL_ZEROS_LITERAL | (1 << maxLiteralLengthModulus(i));
			return;
		}
		
		// position of the next bit to set within the current literal
		int bit = maxLiteralLengthModulus(last) + i - last;

		// if we are outside the current literal, add zeros in
		// between the current word and the new 1-bit literal word
		if (bit >= MAX_LITERAL_LENGHT) {
			int zeroBlocks = maxLiteralLengthDivision(bit) - 1;
			bit = maxLiteralLengthModulus(bit);
			if (zeroBlocks == 0) {
				ensureCapacity(lastWordIndex + 1);
			} else {
				ensureCapacity(lastWordIndex + 2);
				appendFill(zeroBlocks, 0);
			}
			appendLiteral(ALL_ZEROS_LITERAL | 1 << bit);
		} else {
			words[lastWordIndex] |= 1 << bit;
			if (words[lastWordIndex] == ALL_ONES_LITERAL) {
				lastWordIndex--;
				appendLiteral(ALL_ONES_LITERAL);
			}
		}

		// update other info
		last = i;
		if (size >= 0)
			size++;
	}

  /**
   * Append a literal word after the last word
   *
   * @param word the new literal word. Note that the leftmost bit <b>must</b>
   *             be set to 1.
   */
  void ConciseSet::appendLiteral(int word)
  {
    // when we have a zero sequence of the maximum lenght (that is,
    // 00.00000.1111111111111111111111111 = 0x01FFFFFF), it could happen
    // that we try to append a zero literal because the result of the given operation must be an
    // empty set. Whitout the following test, we would have increased the
    // counter of the zero sequence, thus obtaining 0x02000000 that
    // represents a sequence with the first bit set!
    if (lastWordIndex == 0 && word == ConciseSetUtils.ALL_ZEROS_LITERAL && words[0] == 0x01FFFFFF) {
      return;
    }

    // first addition
    if (lastWordIndex < 0) {
      words[lastWordIndex = 0] = word;
      return;
    }

    final int lastWord = words[lastWordIndex];
    if (word == ConciseSetUtils.ALL_ZEROS_LITERAL) {
      if (lastWord == ConciseSetUtils.ALL_ZEROS_LITERAL) {
        words[lastWordIndex] = 1;
      } else if (isZeroSequence(lastWord)) {
        words[lastWordIndex]++;
      } else if (!simulateWAH && containsOnlyOneBit(getLiteralBits(lastWord))) {
        words[lastWordIndex] = 1 | ((1 + Integer.numberOfTrailingZeros(lastWord)) << 25);
      } else {
        words[++lastWordIndex] = word;
      }
    } else if (word == ConciseSetUtils.ALL_ONES_LITERAL) {
      if (lastWord == ConciseSetUtils.ALL_ONES_LITERAL) {
        words[lastWordIndex] = ConciseSetUtils.SEQUENCE_BIT | 1;
      } else if (isOneSequence(lastWord)) {
        words[lastWordIndex]++;
      } else if (!simulateWAH && containsOnlyOneBit(~lastWord)) {
        words[lastWordIndex] = ConciseSetUtils.SEQUENCE_BIT | 1 | ((1 + Integer.numberOfTrailingZeros(~lastWord))
                                                                   << 25);
      } else {
        words[++lastWordIndex] = word;
      }
    } else {
      words[++lastWordIndex] = word;
    }
  }

	/**
	 * Append a sequence word after the last word
	 * 
	 * @param length
	 *            sequence length
	 * @param fillType
	 *            sequence word with a count that equals 0
	 */
	void ConciseSet::appendFill(int length, int fillType) {
		assert length > 0;
		assert lastWordIndex >= -1;
		
		fillType &= SEQUENCE_BIT;
		
		// it is actually a literal...
		if (length == 1) {
			appendLiteral(fillType == 0 ? ALL_ZEROS_LITERAL : ALL_ONES_LITERAL);
			return;
		} 

		// empty set
		if (lastWordIndex < 0) {
			words[lastWordIndex = 0] = fillType | (length - 1);
			return;
		} 
		
		final int lastWord = words[lastWordIndex];
		if (isLiteral(lastWord)) {
			if (fillType == 0 && lastWord == ALL_ZEROS_LITERAL) {
				words[lastWordIndex] = length;
			} else if (fillType == SEQUENCE_BIT && lastWord == ALL_ONES_LITERAL) {
				words[lastWordIndex] = SEQUENCE_BIT | length;
			} else if (!simulateWAH) {
				if (fillType == 0 && containsOnlyOneBit(getLiteralBits(lastWord))) {
					words[lastWordIndex] = length | ((1 + Integer.numberOfTrailingZeros(lastWord)) << 25);
				} else if (fillType == SEQUENCE_BIT && containsOnlyOneBit(~lastWord)) {
					words[lastWordIndex] = SEQUENCE_BIT | length | ((1 + Integer.numberOfTrailingZeros(~lastWord)) << 25);
				} else {
					words[++lastWordIndex] = fillType | (length - 1);
				}
			} else {
				words[++lastWordIndex] = fillType | (length - 1);
			}
		} else {
			if ((lastWord & 0xC0000000) == fillType)
				words[lastWordIndex] += length;
			else
				words[++lastWordIndex] = fillType | (length - 1);
		}
	}

#endif  // COMPRESSED_SET_H__