// g++ --std=gnu++0x CompressedSet.cpp

#include "varint/CompressedSet.h"
#include "varint/bitpacking/memutil.h"
#include <vector>
#include <set>
#include <assert.h>
#include <iostream>

using namespace std;

bool testvec(set<unsigned int>& data)
{
	stringstream ss;
	{
      CompressedSet myset2;
      for (auto i : data)
	  {
	    myset2.addDoc(i);
      }
	  // cout << "added " << data.size() << endl;
	  // myset2.flush();
      myset2.compact();
      myset2.write(ss);
	}

	CompressedSet myset1;
	myset1.read(ss);

	assert(data.size() == myset1.size());
	CompressedSet::Iterator it2(&myset1);
	for (auto idx : data)
	{
		assert(it2.nextDoc()!=NO_MORE_DOCS );
		assert(it2.docID() == idx);
	}
	assert(it2.nextDoc() == NO_MORE_DOCS);

	return true;
}

void test(){

	for (uint32_t b = 0; b <= 28; ++b) {
        cout << "testing1... b = " << b << endl;
        for (size_t length = 128; length < (1U << 12); length += 128) {
	        //cout << "   length = " << length << endl;
	        set<unsigned int> data;
	        for (size_t i = 0; i < length; ++i) {
                unsigned int x = (i + (24 - i) * (12 - i)) % (1U << b);
				data.insert(x);
            }
			if (!testvec(data)) {
				return;
			}
        }
        cout << "testing2... b = " << b << endl;
       	for (size_t length = 1; length < (1U << 9); ++length) {
	      //  cout << "   length = " << length << endl;
		    set<unsigned int> data;
	        for (size_t i = 0; i < length; ++i) {
	           data.insert((33231 - i + i * i) % (1U << b));
	        }
	        if (!testvec(data)) {
				return;
			}
		}
	}
	cout << "All test passed succesfully!!" << endl;
}


int main() {
	test();
}
