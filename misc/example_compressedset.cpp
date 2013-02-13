// from  http://www.stepanovpapers.com/CIKM_2011.pdf 
// compile with g++  -std=gnu++0x -O3 main.cpp  -o result.bin
using namespace std;

#include <iostream>
#include <stdio.h>
#include <assert.h>
#include "varint/CompressedSet.h"
#include <time.h>
#include "varint/LazyAndSet.h"
#include <vector>

double diffclock(clock_t clock1,clock_t clock2)
{
	double diffticks=clock1-clock2;
	double diffms=(diffticks*1000)/CLOCKS_PER_SEC;
	return diffms;
}
void benchmark(){
	int test;
		CompressedSet myset1;
		for (unsigned int i = 1; i<=384000000; ++i){
		    myset1.addDoc(i);	
		}
		std::cout << "compressed!" << endl;
        
		myset1.flush();
		myset1.compact();

		CompressedSet::Iterator it(&myset1);
		// Sequential scanning 384 000 000 docs per second
		clock_t begin=clock();
		for (; it.docID() != NO_MORE_DOCS;it.nextDoc()){
			unsigned int temp = it.docID();
		}
		clock_t end=clock();
	    std::cout << "Iteration Time: " << double(diffclock(end,begin)) << " ms"<< endl;
}



int main() {
	benchmark();
}

