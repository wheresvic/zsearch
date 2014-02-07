#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <memory>
#include <exception>
#include <iostream>
#include <iterator>

#include "lib/tpunit++.hpp"
#include "TestUtils.hpp"
#include "varint/CompressedSet.h"
#include "varint/LazyAndSet.h"
#include <vector>
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
		TEST(CompressedSetTest::testAdvance)

	)
	{}
	
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
		cout << it2.Advance(2) << endl;
		cout << it2.Advance(2) << endl;		
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
	
	void testOrSet()
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