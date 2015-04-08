
#include <sys/file.h>

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <set>
#include <unordered_set>
#include <sstream>
#include <thread>
#include <chrono>

#include "DocumentImpl.hpp"
#include "TokenizerImpl.hpp"
#include "DocumentKVStore.hpp"
#include "NameSpaceKVStore.hpp"
#include "KVStoreLevelDb.hpp"
#include "KVStoreInMemory.hpp"
#include "Engine.hpp"
#include "Constants.hpp"
#include "ZUtil.hpp"
#include "varint/CompressedSet.h"
#include "varint/BasicSet.h"
#include "varint/SetFactory.h"
#include "varint/BasicSetFactory.h"

using namespace std;
    inline bool fast_getline(std::string& line, FILE* input = stdin)
    {
        line.clear();
        static const size_t max_buffer = 65536;
        char buffer[max_buffer];
        bool done = false;
        while (!done) {
            if (!fgets(buffer, max_buffer, input)) {
                if (!line.size()) {
                    return false;
                } else {
                    done = true;
                }
            }
            line += buffer;
            if (*line.rbegin() == '\n') {
                done = true;
            }
        }
        return true;
    }


void search(const string& query, const Engine& engine, unsigned int start, unsigned int offset)
{
	cout << "searching for: " << query << " with start = " << start << ", offset = " << offset << endl;

	auto docIdSet = engine.search(query, start, offset);

	/*
	auto docSet = engine.getDocs(docIdSet);

	for (auto document : docSet)
	{
		string title;
		document->getEntry("title", title);
		cout << title << " ";
	}
	*/

	for (auto docId : docIdSet)
	{
		cout << docId << " ";
	}

	cout << endl;
}


void work(string fileName, bool destroyDb)
{
    FILE * file = fopen(fileName.c_str() , "r");

	//if (!f.is_open())
	//{
	//	
	//	cerr << "unable to open file :(" << endl;
	//	return;
	//}
		string input;

		char documentDelimiter = ',';
		int documentId = 1;

		shared_ptr<ISetFactory> setFactory = make_shared<SetFactory>();


		shared_ptr<ITokenizer> tokenizer = make_shared<TokenizerImpl>();

		shared_ptr<KVStore::IKVStore> storeKV = make_shared<KVStore::KVStoreLevelDb>(zsearch::LEVELDB_TEST_STORE, destroyDb);
		
		storeKV->Open();

		shared_ptr<KVStore::IKVStore> engineDataStore = make_shared<KVStore::NameSpaceKVStore>('e', storeKV);
		shared_ptr<KVStore::IKVStore> fieldStore = make_shared<KVStore::NameSpaceKVStore>('f', storeKV);
		shared_ptr<KVStore::IKVStore> documentStore = make_shared<KVStore::NameSpaceKVStore>('d', storeKV);
		shared_ptr<KVStore::IKVStore> wordIndexStore = make_shared<KVStore::NameSpaceKVStore>('w', storeKV);
		shared_ptr<KVStore::IKVStore> invertedIndexStore = make_shared<KVStore::NameSpaceKVStore>('i', storeKV);

		Engine engine(engineDataStore, fieldStore, documentStore, wordIndexStore, invertedIndexStore, setFactory);

		engine.setMaxBatchSize(100000);

		cout << "Made engine!" << endl;
        while (fast_getline(input, file))
		{    
			documentId++;
			shared_ptr<IDocument> doc = make_shared<DocumentImpl>(); // (new DocumentImpl());
            int fieldId = 0;
            bool isfound = true;
            size_t oldpos = 0;

          do{
            string fieldstr;
            ZUtil::PutUint64(fieldstr,fieldId++);
            const size_t newpos = input.find_first_of(documentDelimiter,oldpos);
            const string& value = input.substr(oldpos, newpos-oldpos);
            doc->addEntry(fieldstr, value); // 25%
            isfound = newpos != string::npos;
            oldpos = newpos+1;
          } while (isfound  );
          
          engine.addDocument(doc);
  	    }
  
    cout << "Done" << endl;

	// flush
	engine.flushBatch();

    //storeKV->Compact();
		// test that searching for some more text returns only 1 document

	//	string query = "1987 SFO 1732";
	//	search(query, engine, 1, 0);
}

int main(int argc, char **argv)
{
	int pid_file = open(zsearch::LOCK_FILE.c_str(), O_CREAT | O_RDWR, 0666);

	int rc = flock(pid_file, LOCK_EX | LOCK_NB);

	if (rc)
	{
		if (EWOULDBLOCK == errno)
		{
			std::cerr << "Only one instance of zsearch is allowed!" << std::endl;
			return -1;
		}

	}

	if (argc < 3)
	{
		cerr << argv[0] << " <input> <destroyDb = 0/1>" << endl;
		return 1;
	}

	string fileName = argv[1];

	bool destroyDb = false;
	string strDestroyDb = argv[2];
	if ("0" != strDestroyDb)
	{
		destroyDb = true;
	}
	work(fileName, destroyDb);
	// work(fileName);

	// std::this_thread::sleep_for(std::chrono::seconds(30));

	return 0;
}

