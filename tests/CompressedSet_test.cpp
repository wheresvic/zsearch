#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <memory>
#include <exception>
#include <iostream>
#include <iterator>
#include <set>
#include <stdio.h>
#include <stdlib.h>
#include <cmath> 
#include <climits>
#include "lib/tpunit++.hpp"
#include "TestUtils.hpp"
#include "varint/CompressedSet.h"
#include "varint/LazyAndSet.h"
#include "varint/LazyOrSet.h"
#include "varint/LazyAndNotSet.h"

#include <vector>
#include <algorithm>
#include <random>
using namespace std;
/**
 * Test DocumentImpl
 */
struct CompressedSetTest : tpunit::TestFixture
{
	CompressedSetTest() : tpunit::TestFixture
	(
		TEST(CompressedSetTest::testAddAfterRead),
		TEST(CompressedSetTest::testEmptySet),
		TEST(CompressedSetTest::testAndSet),
		TEST(CompressedSetTest::testAnd2Set),
		TEST(CompressedSetTest::testAdvance),
		TEST(CompressedSetTest::testCorrectness),
		TEST(CompressedSetTest::ANDNOTtest),
		TEST(CompressedSetTest::ANDNOTtest2),
		TEST(CompressedSetTest::ANDtest),
		TEST(CompressedSetTest::testOrSet),
		TEST(CompressedSetTest::testOr2Set)
		
	){}

// https://github.com/javasoze/kamikaze/blob/master/src/test/java/com/kamikaze/test/PForDeltaKamikazeTest.java

	void testCorrectness()
	{
		int numdocs = 257;
	    std::default_random_engine generator;
        std::uniform_int_distribution<unsigned int> distribution(1,UINT_MAX);

	    set<unsigned int> set;
	    while(set.size() < numdocs){
	      set.insert(distribution(generator));
	    }
	    
	    unsigned int* numArray = new unsigned int[set.size()];    
	    vector<unsigned int> v;
	    for (unsigned int n : set){
	      v.push_back(n);
	    }
	    std::sort(v.begin(),v.end());

	    CompressedSet docset;
	    for (int n : v){
	      docset.addDoc(n); 
	    }

	    shared_ptr<Set::Iterator> it = docset.iterator();
	    unsigned int doc;
	    int i = 0;
	    while((doc=it->nextDoc()) != NO_MORE_DOCS){
	    	ASSERT_EQUAL(v[i++],doc);
	    }
	    delete[] numArray;
	}

	void testEmptySetIterator() 
	{
	    shared_ptr<Set> docset = make_shared<CompressedSet>();
	    shared_ptr<Set::Iterator> it = docset->iterator();
	    ASSERT_TRUE(it->nextDoc() == NO_MORE_DOCS);
	}

	void ANDNOTtest() {
		shared_ptr<Set> docset1 = make_shared<CompressedSet>();
	    shared_ptr<Set> docset2 = make_shared<CompressedSet>();
	    for (int i = 1; i < 200000; i += 4)
            docset2->addDoc(i);
        for (int i = 200000; i < 400000; i += 14)
            docset2->addDoc(i);
	    
        shared_ptr<Set> andNotresult = make_shared<LazyAndNotSet>(docset1,docset2);
        shared_ptr<Set> off = make_shared<LazyAndNotSet>(docset2,docset1);
        ASSERT_TRUE(andNotresult->equals(docset1));
        ASSERT_TRUE(off->equals(docset2));
        shared_ptr<Set> last = make_shared<LazyAndNotSet>(docset2,docset1);
        ASSERT_TRUE(last->equals(off));
	}

	void ANDNOTtest2() {
		shared_ptr<Set> docset1 = make_shared<CompressedSet>();
	    shared_ptr<Set> docset2 = make_shared<CompressedSet>();
	    docset1->addDoc(10);
	    docset1->addDoc(70);
	    docset1->addDoc(100);
	    docset1->addDoc(150);
	    docset1->addDoc(15000);
	    
        docset2->addDoc(11);
	    docset2->addDoc(13);
	    docset2->addDoc(100);
	    docset2->addDoc(15000);

        shared_ptr<Set> andNotresult = make_shared<LazyAndNotSet>(docset1,docset2);

        ASSERT_EQUAL(andNotresult->size(),3);
        ASSERT_TRUE(andNotresult->find(10));
        ASSERT_TRUE(andNotresult->find(70));
        ASSERT_TRUE(!andNotresult->find(100));
        ASSERT_TRUE(andNotresult->find(150));
        ASSERT_TRUE(!andNotresult->find(15000));
	}




	void ANDtest() {
		        shared_ptr<Set> docset = make_shared<CompressedSet>();
		        for (int k = 1; k < 4000; ++k) {
                     docset->addDoc(k);
                }
                docset->addDoc(100000);
                docset->addDoc(110000);


                shared_ptr<Set> docset2 = make_shared<CompressedSet>();
                docset2->addDoc(13);


                shared_ptr<Set> intersect = make_shared<LazyAndSet>(docset,docset2);
                shared_ptr<Set::Iterator> it = intersect->iterator();
                ASSERT_EQUAL(it->nextDoc(),13);
                ASSERT_TRUE(intersect->find(13));
                ASSERT_EQUAL(intersect->size(),1);           
	}

	void testAdvance()
	{
		CompressedSet myset1;
		for (int i = 3; i<100000; i++){
			myset1.addDoc(i);
		}

		stringstream ss;
		myset1.write(ss);
		
		CompressedSet myset2;
		myset2.read(ss);
		CompressedSet::Iterator it2(&myset2);
		it2.Advance(2);
		it2.Advance(2);
	}

	void testAddAfterRead()
	{
		CompressedSet myset1;
		myset1.addDoc(1);
		stringstream ss;
		myset1.write(ss);
		
		CompressedSet myset2;
		myset2.read(ss);
		myset2.addDoc(79);
		
		if(!(myset2.find(1) && myset2.find(79))){
			TRACE("can't find docid 1 and 79");
			ABORT();
		}
		if(myset2.find(2)){
			TRACE("should not contain 2");
			ABORT();
		}
		
		CompressedSet::Iterator it2(&myset2);
		it2.nextDoc();
		ASSERT_EQUAL(it2.docID(),1);
		it2.nextDoc();
		ASSERT_EQUAL(it2.docID(),79);		
	}
	
	void testEmptySet()
	{
		CompressedSet myset1;
		stringstream ss;
		myset1.write(ss);
		
		CompressedSet myset2;
		myset2.read(ss);
		CompressedSet::Iterator it2(&myset2);

		ASSERT_EQUAL(it2.nextDoc(),NO_MORE_DOCS);
		ASSERT_EQUAL(it2.docID(),NO_MORE_DOCS);
	}
	
	void testAndSet()
	{
		shared_ptr<Set> myset1 = make_shared<CompressedSet>();
		myset1->addDoc(1);
		myset1->addDoc(3);
		myset1->addDoc(5);
		
		shared_ptr<Set> myset2 = make_shared<CompressedSet>();
		myset2->addDoc(2);
		myset2->addDoc(3);
		myset2->addDoc(4);
		
		vector<shared_ptr<Set>> intersectionSet;
		intersectionSet.push_back(myset1);
		intersectionSet.push_back(myset2);
		
		LazyAndSet andSet(intersectionSet);
		shared_ptr<Set::Iterator> it = andSet.iterator();
		ASSERT_EQUAL(it->nextDoc(),3);
		ASSERT_EQUAL(it->docID(),3);
	}

    void testAnd2Set()
	{
		std::vector<shared_ptr<Set>> DocList;
		std::vector<unsigned int> intSet;
	    for (int i=0;i<5;++i)
	    {
	      shared_ptr<Set> docset = make_shared<CompressedSet>();
	      docset->addDoc(5);
	      intSet.push_back(5);
	      docset->addDoc((i+1)*100);
	      intSet.push_back((i+1)*100);
	      DocList.push_back(docset);
	    }
	    shared_ptr<Set> andset = make_shared<LazyAndSet>(DocList);

        shared_ptr<Set::Iterator> it = andset->iterator();
	    int doc;
        while((doc=it->nextDoc())!=NO_MORE_DOCS)
        {
       	  ASSERT_TRUE(std::find(intSet.begin(), intSet.end(), doc) != intSet.end());
        }
	}

	
	void testOrSet()
	{
		shared_ptr<Set> docset1 = make_shared<CompressedSet>();
	    shared_ptr<Set> docset2 = make_shared<CompressedSet>();
	    docset1->addDoc(10);
	    docset1->addDoc(70);
	    docset1->addDoc(100);
	    docset1->addDoc(150);
	    docset1->addDoc(15000);
	    
        docset2->addDoc(11);
	    docset2->addDoc(13);
	    docset2->addDoc(100);
	    docset2->addDoc(15000);
	
		shared_ptr<Set> unionSet = make_shared<LazyOrSet>(docset1,docset2);
	    ASSERT_EQUAL(unionSet->size(),7);
	    
	    std::vector<unsigned int> expected = {10, 70, 100, 150, 15000, 11, 13};
	    for (auto value : expected){
			ASSERT_TRUE(unionSet->find(value));
	    }

	    std::vector<unsigned int> expected2 = {10, 11, 13, 70, 100, 150, 15000};
	    shared_ptr<Set::Iterator> it = unionSet->iterator();
	    for (auto value : expected2){
			ASSERT_TRUE(it->nextDoc()!=NO_MORE_DOCS );
			ASSERT_TRUE(it->docID() == value);
	    }
	    ASSERT_TRUE(it->nextDoc() == NO_MORE_DOCS);
	}

	void testOr2Set()
	{
		std::vector<shared_ptr<Set>> DocList;
		std::vector<unsigned int> intSet;
	    for (int i=0;i<5;++i)
	    {
	      shared_ptr<Set> docset = make_shared<CompressedSet>();
	      docset->addDoc((i+1)*10);
	      intSet.push_back((i+1)*10);
	      docset->addDoc((i+1)*100);
	      intSet.push_back((i+1)*100);
	      DocList.push_back(docset);
	    }
	    shared_ptr<Set> orset = make_shared<LazyOrSet>(DocList);

        shared_ptr<Set::Iterator> it = orset->iterator();
	    int doc;
        while((doc=it->nextDoc())!=NO_MORE_DOCS)
        {
       	  ASSERT_TRUE(std::find(intSet.begin(), intSet.end(), doc) != intSet.end());
        }
	}


	
};

int main()
{
	CompressedSetTest test;

	/**
	 * Run all of the registered tpunit++ tests. Returns 0 if
	 * all tests are successful, otherwise returns the number
	 * of failing assertions.
	 */
	return tpunit::Tests::Run();

}