// from  http://www.stepanovpapers.com/CIKM_2011.pdf 
// compile with g++  -funroll-loops -ftree-vectorize -O3 -mssse3 main.cpp  -o result.bin
// g++  -g -O3 -mssse3 main.cpp  -o result.bin

//g++ -m64 -std=gnu++0x main.cpp -I ./ libleveldb.a -o result.bin
using namespace std;

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

#include <stdio.h>

#include "varint/CompressedSet.h"
#include "varint/LazyAndSet.h"

#include <time.h>
#include <stdint.h>

#include <vector>
#include "leveldb/db.h"
#include "leveldb/cache.h"

char* EncodeVarint64(char* dst, uint64_t v) {
  static const int B = 128;
  unsigned char* ptr = reinterpret_cast<unsigned char*>(dst);
  while (v >= B) {
    *(ptr++) = (v & (B-1)) | B;
    v >>= 7;
  }
  *(ptr++) = static_cast<unsigned char>(v);
  return reinterpret_cast<char*>(ptr);
}

void PutVarint64(std::string* dst, uint64_t v) {
  char buf[10];
  char* ptr = EncodeVarint64(buf, v);
  dst->append(buf, ptr - buf);
}

int VarintLength(uint64_t v) {
  int len = 1;
  while (v >= 128) {
    v >>= 7;
    len++;
  }
  return len;
}

vector<char> encodeVarInt(uint64_t n)
{
    std::vector<char> ret(10); 

    do {
        unsigned char c = n & 0x7f;
        n >>= 7;
        if (n)
            c |= 0x80;
        ret.push_back(c);
    } while (n);

    return ret;
}


int64_t decodeVarInt(const char *  p,const char * limit)
{
	
    int64_t foundInt = 0;
    int shift = 0;
    do {
        if (p >= limit)
            return 0;

        unsigned char c = *p;
        foundInt |= static_cast<int64_t>(c & 0x7f) << shift;
        shift += 7;
    } while (*p++ & 0x80);
    return foundInt;
}

double diffclock(clock_t clock1,clock_t clock2)
{
	double diffticks=clock1-clock2;
	double diffms=(diffticks*1000)/CLOCKS_PER_SEC;
	return diffms;
}

void benchmark(){
		

	
	
		shared_ptr<CompressedSet> myset1(new CompressedSet());
		for (unsigned int i = 0; i<=37000000; ++i){
		  myset1->addDoc(i);	
		}
	
		shared_ptr<CompressedSet> myset2(new CompressedSet());
		for (unsigned int i = 0; i<=370000000; ++i){
		  myset2->addDoc(i);	
		}
		vector<shared_ptr<Set> > vec;
		vec.push_back(myset2);
		vec.push_back(myset1);
		LazyAndSet andSet(vec);


	    clock_t begin=clock();
		Iterator* it;
		shared_prt<Iterator> it;
		
		for (shared_ptr<Set::Iterator> it = andSet.iterator();
		    it->docID() != NO_MORE_DOCS;
		    it->nextDoc()) {
		}
		clock_t end=clock();
				
		std::cout << "Iteration Time Decompression: " << double(diffclock(end,begin)) << " ms"<< endl;

}

string convertInt(int number)
{
   stringstream ss;//create a stringstream
   ss << number;//add number to the stream
   return ss.str();//return a string with the contents of the stream
}
void printSet(CompressedSet& set){
	CompressedSet::Iterator it(&set);
	while(it.nextDoc() != NO_MORE_DOCS){
		std::cout << it.docID() << endl;
	}
}

int processLevelDB0(){
	int64_t count = 1;
	
	leveldb::DB* db;
    leveldb::DB* db2;
	leveldb::Options options;
    options.create_if_missing = true;
	leveldb::Status status1 = leveldb::DB::Open(options, "./url", &db);
	leveldb::Status status2 = leveldb::DB::Open(options, "./url2", &db2);
	

	

	
	ifstream file("dictionary3");
	std::string line;
	string word;
	string url;
	string oldurl;
	bool isnew = true;
	while (std::getline(file, line))
    {
	    stringstream ss(line);
     	std::getline(ss, url,'\t');
		std::getline(ss, word);
        if(isnew){
			oldurl = url;
			isnew = false;
			continue;
        }       
        if ( url!= oldurl) {
		
		  vector<char> charvect = encodeVarInt(count);
		  leveldb::Slice s(&charvect[0],charvect.size());
		  leveldb::Status indexStatus = db->Put(leveldb::WriteOptions(), oldurl, s);
		  if (!indexStatus.ok()) {
            cerr << indexStatus.ToString() << endl;
		    return -1;
		  }
		  
		  leveldb::Status indexStatus2 = db2->Put(leveldb::WriteOptions(), s, oldurl);
		  if (!indexStatus2.ok()) {
            cerr << indexStatus2.ToString() << endl;
		    return -1;
		  }
		  oldurl = url;
		  count++;
		}
	}
	if ( url!= oldurl) {
	
	  vector<char> charvect = encodeVarInt(count);
	  leveldb::Slice s(&charvect[0],charvect.size());
	  leveldb::Status indexStatus = db->Put(leveldb::WriteOptions(), oldurl, s);
	  if (!indexStatus.ok()) {
        cerr << indexStatus.ToString() << endl;
	    return -1;
	  }
	  
	  leveldb::Status indexStatus2 = db2->Put(leveldb::WriteOptions(), s, oldurl);
	  if (!indexStatus2.ok()) {
        cerr << indexStatus2.ToString() << endl;
	    return -1;
	  }
	  oldurl = url;
	  count++;
	}
	
	file.close();
	delete db2;
	delete db;
}

int updateWords(leveldb::DB* db,leveldb::DB* db2,std::string& oldurl,std::vector<std::string>& words){
	
                //get url docid from url string
				string idstr;
				leveldb::Status s = db->Get(leveldb::ReadOptions(), oldurl, &idstr);
				if (s.IsNotFound() == true){
					  cerr << s.ToString() << endl;
					  return -1;
			    }
				int docid =  decodeVarInt(&*idstr.begin(),&*idstr.end());
				//cout << "docid: " << docid << endl;
				
				// update all word bitmap
				for(std::vector<std::string>::iterator it = words.begin(); it != words.end(); ++it) {
					std::string tempword = *it;
					
							
					string bitmap;
					leveldb::Status indexStatus = db2->Get(leveldb::ReadOptions(), tempword, &bitmap);

					if (indexStatus.IsNotFound() == true){
						CompressedSet set;
						set.addDoc(docid);
						stringstream ss;
						set.write(ss);
						bitmap = ss.str();
					} else {
						stringstream bitmapStream(bitmap);
						CompressedSet set;
						set.read(bitmapStream);
					//	cout << "update existing index for: " << tempword << endl;

						if (set.lastAdded < docid ) {
							set.addDoc(docid);
							stringstream ss;
							set.write(ss);
							bitmap = ss.str();
						} else {
							stringstream ss;
							CompressedSet set2 = set.unorderedAdd(docid);
							set2.write(ss);
							
							bitmap = ss.str();
						}

					}
					leveldb::WriteOptions write_options;
					write_options.sync = false;
					leveldb::Status indexStatus2 = db2->Put(write_options, tempword, bitmap);
					if (!indexStatus2.ok()) {
			          cerr << indexStatus2.ToString() << endl;
					  return -1;
					}						
				}	
}

int processLevelDB1(){
	  leveldb::DB* db;
	  leveldb::DB* db2;
	  leveldb::Options options1;
	  options1.create_if_missing = true;
 	  options1.block_cache = leveldb::NewLRUCache(189 * 1048576);  // 100MB cache
	  leveldb::Options options2;
	  options2.create_if_missing = true;
      options2.compression = leveldb::kNoCompression;
 	  options2.block_cache = leveldb::NewLRUCache(1000 * 1048576);  // 100MB cache
	  leveldb::Status status1 = leveldb::DB::Open(options1, "./url", &db);
	  leveldb::Status status2 = leveldb::DB::Open(options2, "./reverseIndex", &db2);



		ifstream file("dictionary3");
		std::string line;
		string word;
		string url;
		string oldurl;
		std::vector<std::string> words;
		bool first = true;
		
		while (file.good())
	    {
		
			std::getline(file, line);
			stringstream ss(line);
	     	std::getline(ss, url,'\t');
			std::getline(ss, word);
			
			if (first){
				first = false;
				oldurl = url;

				words.push_back(word);
				continue;
			} 
			

			if (url != oldurl){
				if (updateWords(db,db2,oldurl,words) == -1){
					cout << "error" <<endl;
					return -1;
				}
				words = std::vector<string>();
				oldurl = url;
				words.push_back(word);
			} else {
				words.push_back(word);
			}
	    }
	    if (words.size() > 0 ){
	    	if (updateWords(db,db2,url,words) == -1){
		        cout << "error" <<endl;
				return -1;
	        }
	    } 
	
		cout << "done" <<endl;
		delete db;
		delete db2;
		delete options1.block_cache;
		delete options2.block_cache;
}

int processLevelDB2(){
	  leveldb::DB* db2;
	  leveldb::Options options;
	  options.create_if_missing = true;
	  leveldb::Status status2 = leveldb::DB::Open(options, "./reverseIndex", &db2);


      leveldb::DB* db;
      leveldb::Status status1 = leveldb::DB::Open(options, "./url2", &db);

	  string word;
	  while (true){
		cout <<  std::endl;
		cout << "Type a word" << std::endl;
		std::getline(cin, word);

		string bitmap;
		  leveldb::Status indexStatus = db2->Get(leveldb::ReadOptions(), word, &bitmap);
	      if (indexStatus.IsNotFound() == true){
		       
    			cout << "not found " << endl;
				leveldb::Iterator* it = db2->NewIterator(leveldb::ReadOptions());
				it->SeekToFirst();
				while (it->Valid()){
					cout << it->key().ToString() << endl;
					it->Next();
				}
	      } else {
				stringstream bitmapStream(bitmap);
				CompressedSet set;
				set.read(bitmapStream);
				CompressedSet::Iterator it(&set);
				while(it.nextDoc() != NO_MORE_DOCS){
					vector<char> charvect = encodeVarInt(it.docID());
					leveldb::Slice s(&charvect[0],charvect.size());
					string value;
					leveldb::Status urlStatus = db->Get(leveldb::ReadOptions(), s, &value);
					std::cout << value << endl;
				}
	      }
	  }
	  
	 delete db2;
	
}

void compact(){

	  leveldb::DB* db2;
	  leveldb::Options options2;
	  options2.create_if_missing = true;
      options2.compression = leveldb::kNoCompression;
	  options2.block_cache = leveldb::NewLRUCache(600 * 1048576);  // 100MB cache
	  leveldb::Status status2 = leveldb::DB::Open(options2, "./reverseIndex", &db2);
	  db2->CompactRange(NULL, NULL);
	  delete db2;
      delete options2.block_cache;
}
int main() {
//	processLevelDB0();
  processLevelDB1();
	//reverseList();
//	processLevelDB2();
	//compact();
	return 0;
}




