#include <iostream>
#include <stdio.h>
#include <assert.h>
#include <vector>
#include <memory>
#include <algorithm>

#include "BasicSet.h"

	BasicSet::BasicSet(const BasicSet& other)
	{
	    docs = other.docs;
    }


   /**
    * Swap the content of this bitmap with another bitmap.
    * No copying is done. (Running time complexity is constant.)
    */

    void BasicSet::swap(BasicSet &x)
	{
		assert(false);
    }

    BasicSet::BasicSet()
	{ }

    BasicSet::~BasicSet()
	{ }


    void BasicSet::write(ostream & out)
	{
		for (auto val : docs)
		{
			out << val << " ";
		}
    }

    void BasicSet::read(istream &in)
	{
		docs.clear();
		set<unsigned int>().swap(docs);

		unsigned int docId;
		while(in >> docId)
		{
			docs.insert(docId);
		}
    }

    shared_ptr<Set::Iterator> BasicSet::iterator() const
	{
		shared_ptr<Set::Iterator> it = make_shared<BasicSet::Iterator>(this);
		return it;
    }


	/**
	 * Add document to this set
	 */
	void BasicSet::addDoc(unsigned int docId)
	{
		docs.insert(docId);
	}

	void BasicSet::addDocs(unsigned int* docids,size_t start,size_t len)
	{
		throw -1;
	}

	BasicSet BasicSet::unorderedAdd(unsigned int docId)
	{
		/*
		BasicSet set;
		BasicSet::Iterator it(this);
		bool inserted = false;
		while (it.nextDoc() != NO_MORE_DOCS )
		{
			unsigned int val = it.docID();
			if (val > docId && !inserted)
			{
				inserted = true;
				set.addDoc(docId);
			}

			set.addDoc(val);
		}

		if (!inserted)
		{
			set.addDoc(docId);
		}
		*/

		BasicSet set;
		set.addDoc(docId);

		return set;
	}

	void BasicSet::removeDocId(unsigned int docId)
	{
		docs.erase(docId);
	}

	BasicSet BasicSet::removeDoc(unsigned int docId)
	{
		BasicSet set;
		BasicSet::Iterator it(this);

		while (it.nextDoc() != NO_MORE_DOCS )
		{
			unsigned int val = it.docID();
			if (val != docId)
			{
				set.addDoc(val);
			}
		}
		return set;
	}

	void BasicSet::compact()
	{ }

    /**
     * Gets the number of ids in the set
     * @return docset size
     */
    unsigned int BasicSet::size() const
	{
		return docs.size();
    }

    inline bool BasicSet::find(unsigned int target) const
	{
		if (docs.find(target) != docs.end())
			return true;

		return false;
    }


	BasicSet::Iterator::Iterator(const BasicSet* const parentSet)
	{
		set = parentSet;
		cursor = (parentSet->docs).begin();
	}

	BasicSet::Iterator::Iterator(const BasicSet::Iterator& other)
	{
		cursor = other.cursor;
		set = other.set;
	}

	BasicSet::Iterator& BasicSet::Iterator::operator=(const BasicSet::Iterator& other)
	{
		cursor = other.cursor;
		set = other.set;
		return *this;
	}

	BasicSet::Iterator::~Iterator()
	{

	}

    unsigned int BasicSet::Iterator::nextDoc()
	{
		if (!init)
		{
			init = true;
			return *cursor;;
		}

		if ((cursor != (set->docs).end()) && (++cursor != (set->docs).end()))
		{
			return *cursor;
        }

        return NO_MORE_DOCS;
    }

    unsigned int BasicSet::Iterator::docID()
	{
        if (cursor != (set->docs).end())
			return *cursor;

		return NO_MORE_DOCS;
    }

	// Advances to the first beyond the current
    // whose value is greater than or equal to target.
    unsigned int BasicSet::Iterator::Advance(unsigned int target)
	{
		while (cursor != (set->docs).end())
		{
			if ((*cursor++ >= target) && (cursor != (set->docs).end()))
			{
				return *cursor;
			}
		}

		return NO_MORE_DOCS;
    }


