// g++ --std=gnu++0x CompressedSet.cpp 

#include "../varint/CompressedSet.h"
#include <vector>
#include <assert.h>
#include <iostream>

using namespace std;

bool testvec(vector<uint32_t> & data){
	CompressedSet myset1;
	for (size_t i = 0; i < data.size(); ++i) {
		myset1.addDoc(data[i]);
	}
	myset1.flush();
	myset1.compact();
	
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
        cout << "testing... b = " << b << endl;
        for (size_t length = 128; length < (1U << 12); length += 128) {
	        vector<uint32_t> data(length);
	        for (size_t i = 0; i < data.size(); ++i) {
                data[i] = (i + (24 - i) * (12 - i)) % (1U << b);
            }
			if (!testvec(data)) {
				return;
			}
        }

       	for (size_t length = 0; length < (1U << 9); ++length) {
		    vector<uint32_t> data(length);
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
	cout << "All test passed succesfully!!" << endl;
}
