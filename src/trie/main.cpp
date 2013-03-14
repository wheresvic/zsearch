using namespace std;
#include <vector>
#include <iostream>
#include <sstream>
#include <string>
#include <stdint.h>
#include "EntropyTrie.hpp"

int main() {
	EntropyTrie trie(5);
	vector<string> str;
	str.push_back("aaaaa");
	str.push_back("aaaab");
	str.push_back("azaac");
	str.push_back("azzad");
	str.push_back("azzze");
	str.push_back("zaaaf");
	str.push_back("zazzg");
	str.push_back("zzaah");
	str.push_back("zzzai");
	str.push_back("zzzzj");
	for (string s : str){
		trie.insert(s);
	}
	trie.flush();
	size_t pos = trie.locate("aaaab");
	std::cout << pos << std::endl;
	

	return 0;
}
