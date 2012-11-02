#include "KVStore.hpp"
#include <iostream>
#include <string>
using namespace std;

int main() {
	KVStore::KVStore store;
	store.Open();
	store.Put("hello","world");
	string ret;
	store.Get("helklo",ret);
	std::cout << ret << std::endl;
	return 0;
}