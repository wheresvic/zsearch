
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
	char LocalBuffer[4096];
	std::ios::sync_with_stdio(false);
	ifstream f(fileName.c_str());
    f.rdbuf()->pubsetbuf(LocalBuffer, 4096);

	if (f.is_open())
	{
		string input;

		char documentDelimiter = ' ';
		int documentId = 500;

		shared_ptr<ISetFactory> setFactory = make_shared<SetFactory>();
		// shared_ptr<ISetFactory> setFactory = make_shared<BasicSetFactory>();

		shared_ptr<ITokenizer> tokenizer = make_shared<TokenizerImpl>();

		shared_ptr<KVStore::IKVStore> storeKV = make_shared<KVStore::KVStoreLevelDb>(zsearch::LEVELDB_TEST_STORE, destroyDb);
		storeKV->Open();

		shared_ptr<KVStore::IKVStore> engineDataStore = make_shared<KVStore::NameSpaceKVStore>('e', storeKV);
		shared_ptr<KVStore::IKVStore> fieldStore = make_shared<KVStore::NameSpaceKVStore>('f', storeKV);
		shared_ptr<KVStore::IKVStore> documentStore = make_shared<KVStore::NameSpaceKVStore>('d', storeKV);
		shared_ptr<KVStore::IKVStore> wordIndexStore = make_shared<KVStore::NameSpaceKVStore>('w', storeKV);
		shared_ptr<KVStore::IKVStore> invertedIndexStore = make_shared<KVStore::NameSpaceKVStore>('i', storeKV);

		Engine engine(engineDataStore, fieldStore, documentStore, wordIndexStore, invertedIndexStore, setFactory);

		// engine.disableBatching();
		engine.setMaxBatchSize(100000);

		cout << "Made engine!" << endl;

		while (getline(f, input))
		{
			// cout << input;

			/*
			if (documentId > 1000)
				break;
			*/

			string title = ZUtil::getString(documentId++);
			shared_ptr<IDocument> doc = make_shared<DocumentImpl>(); // (new DocumentImpl());
			// doc->setTitle(title);
			doc->addEntry("t", title);

			// parse the input, each line is a single document
			size_t found = input.find_first_of(documentDelimiter);
			if (found != string::npos)
			{
				string field = input.substr(0, found);
				string value = input.substr(found + 1);
				// cout << "field : " << field << ", value: " << value << endl;
				// doc->addEntry("document", value);
				doc->addEntry("f", value);
			}
			else
			{
				throw "Couldn't split key value!";
			}

			// doc->addEntry("memtest", input);
			//engine.addDocument(doc);
			//engine.addDocument(doc);
			cout << "Added document: " << engine.addDocument(doc) << endl;
		}

		f.close();

		// flush
//		engine.flushBatch();
/*
        storeKV->Compact();
		// test that searching for some more text returns only 1 document

		string query = "some  more text";
		search(query, engine, 0, 0);

		// engine.deleteDocument(2);	// deletes some more text
		search(query, engine, 0, 0);

		query = "série";
		search(query, engine, 0, 0);

		query = "de";
		search(query, engine, 0, 0);
		search(query, engine, 0, 1);
		search(query, engine, 1, 1);
		search(query, engine, 5, 5);
		search(query, engine, 4, 7);
		search(query, engine, 2, 0);
			*/
	}
	else
	{
		cerr << "unable to open file :(" << endl;
	}

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
	int iDestroyDb = ZUtil::getInt(strDestroyDb);

	if (iDestroyDb)
	{
		destroyDb = true;
	}

	work(fileName, destroyDb);
	// work(fileName);

	// std::this_thread::sleep_for(std::chrono::seconds(30));

	return 0;
}

