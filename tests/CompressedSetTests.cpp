// g++ --std=gnu++0x CompressedSet.cpp 

#include "../varint/CompressedSet.h"
#include "../varint/bitpacking/memutil.h"
#include <vector>
#include <assert.h>
#include <iostream>

using namespace std;

bool testvec(vector<uint32_t,AlignedSTLAllocator<uint32_t, 64>> & data){
	if (needPaddingTo128Bits(&data[0])){
		cout << "test failed because of bad allignement" << endl;
	}
	stringstream ss;
	{
      CompressedSet myset2;
      for (size_t i = 0; i < data.size(); ++i) {
      	myset2.addDoc(data[i]);
      }
      myset2.flush();
      myset2.compact();
      myset2.write(ss);
	}

	CompressedSet myset1;
	myset1.read(ss);


	assert(data.size() == myset1.size());
	CompressedSet::Iterator it2(&myset1);
	for (int idx = 0; idx < data.size();++idx){
		assert(it2.nextDoc()!=NO_MORE_DOCS );
		assert(it2.docID() == data[idx]);
	}
	assert(it2.nextDoc() == NO_MORE_DOCS);

	return true;
}

void test(){
	
	for (uint32_t b = 0; b <= 28; ++b) {
        cout << "testing1... b = " << b << endl;
        for (size_t length = 128; length < (1U << 12); length += 128) {
	        //cout << "   length = " << length << endl;
	        vector<uint32_t,AlignedSTLAllocator<uint32_t, 64>> data(length);
	        for (size_t i = 0; i < data.size(); ++i) {
                data[i] = (i + (24 - i) * (12 - i)) % (1U << b);
            }
			if (!testvec(data)) {
				return;
			}
        }
        cout << "testing2... b = " << b << endl;
       	for (size_t length = 1; length < (1U << 9); ++length) {
	      //  cout << "   length = " << length << endl;
		    vector<uint32_t,AlignedSTLAllocator<uint32_t, 64>> data(length);
	        for (size_t i = 0; i < data.size(); ++i) {
	           data[i] = (33231 - i + i * i) % (1U << b);
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
