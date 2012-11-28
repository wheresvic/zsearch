/**
 * This is code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 *
 * (c) Daniel Lemire, http://lemire.me/en/
 */
#include <stdlib.h>
#include "headers/ewah.h"
#include <set>
#include <cstdlib> 
#include <ctime>
#include <vector>
//EWAHBoolArray<uint64_t>
// EWAHBoolArray<uint32_t> bitset1;
using namespace std;

vector<unsigned int> getvec(int density,int length){
	
	vector<unsigned int> temp;
	int qty = length / density;
	set<unsigned int> myset;
	//while (myset.size() < qty) {		
	//	int pos = (rand()%length)+1;
//		myset.insert(pos);
//	}
	for (int i = 1; i<= qty; i++){
		myset.insert(i);
	}
	for (auto it=myset.begin(); it!=myset.end(); it++){
		temp.push_back(*it);
	}

	return temp;	
}

double getSize(int density,int length){
	int iterCount = 500;
	unsigned int total = 0;
	int iter = 0;
	for (int i = 0; i<iterCount; i++){
	 stringstream ss;
	 EWAHBoolArray<uint32_t> set;
     vector<unsigned int> v =getvec(density,length);
	 for (auto value :  v) {
		set.set(value);
	 }
	 set.write(ss);
	 total += ss.str().size();
	 iter++;
	}
	return ( ( (total/iter) * 8 ) * 1.0 )/ ((length/density) * 1.0) ;
}

void f(){
	for (int i=1; i<=32; i++){
		cout << "Density = 1/" << i << endl;
		cout << "size = " << getSize(i,5000) << endl;
	}

}

int main() {
	srand((unsigned)time(0)); 
	f();
}
