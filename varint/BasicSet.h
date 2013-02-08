#ifndef BASIC_SET_H__
#define BASIC_SET_H__

#include "Set.h"
#include <set>
#include <limits>

using namespace std;

class BasicSet;

class BasicSet : public Set
{

	public:

		class Iterator : public Set::Iterator
		{
			private:
				set<unsigned int>::iterator cursor;
				// parent
				const BasicSet* set;

				bool init = false;

			public:

				Iterator(const BasicSet* parentSet);
				Iterator(const BasicSet::Iterator& other);

				BasicSet::Iterator& operator=(const BasicSet::Iterator& rhs);
				~Iterator();

			    unsigned int docID();
			    unsigned int nextDoc();
				unsigned int Advance(unsigned int target);

		};

	private:

		const BasicSet& operator=(const BasicSet& other);

		set<unsigned int> docs;

	public:

		BasicSet(const BasicSet& other);

		/**
		 * Swap the content of this bitmap with another bitmap.
		 * No copying is done. (Running time complexity is constant.)
		 */
		void swap(BasicSet & x);


		BasicSet();

		~BasicSet();

		void write(ostream & out);

		void read(istream & in);

		shared_ptr<Set::Iterator> iterator() const;


		/**
		 * Add document to this set
		 * Note that you must set the bits in increasing order:
		 * addDoc(1), addDoc(2) is ok;
		 * addDoc(2), addDoc(1) is not ok.
		 */
		void addDoc(unsigned int docId);

		void addDocs(unsigned int* docids,size_t start,size_t len);

		BasicSet unorderedAdd(unsigned int docId);

		void removeDocId(unsigned int docId);

		BasicSet removeDoc(unsigned int docId);

		void compact();

		void initSet();

		/**
		 * Gets the number of ids in the set
		 * @return docset size
		 */
		unsigned int size() const;

		//This method will not work after a call to flush()
		bool find(unsigned int target) const;

};

#endif
