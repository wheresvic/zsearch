// g++ -std=c++0x simple_leveldb_example.cpp -I ./include libleveldb.a -pthread

#include <iostream>
#include <sstream>
#include <string>

#include "leveldb/db.h"

using namespace std;

int main(int argc, char** argv)
{
    // Set up database connection information and open database
    leveldb::DB* db;
    leveldb::Options options;
    options.create_if_missing = true;

    leveldb::Status status = leveldb::DB::Open(options, "./testdb", &db);

    if (false == status.ok())
    {
        cerr << "Unable to open/create test database './testdb'" << endl;
        cerr << status.ToString() << endl;
        return -1;
    }

    // Add 256 values to the database
    leveldb::WriteOptions writeOptions;
    for (unsigned int i = 0; i < 256; ++i)
    {
        ostringstream keyStream;
        keyStream << "Key" << i;

        ostringstream valueStream;
        valueStream << "Test data value: " << i;

        db->Put(writeOptions, keyStream.str(), valueStream.str());
    }

    // Iterate over each item in the database and print them
    leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());

    for (it->SeekToFirst(); it->Valid(); it->Next())
    {
        cout << it->key().ToString() << " : " << it->value().ToString() << endl;
    }

    if (false == it->status().ok())
    {
        cerr << "An error was found during the scan" << endl;
        cerr << it->status().ToString() << endl;
    }

    delete it;

    // Close the database
    delete db;
}
