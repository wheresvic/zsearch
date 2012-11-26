#ifndef WORDINDEX_H
#define WORDINDEX_H
#include <string>
#include <string.h> // for strcmp
#include <tr1/functional> // for std::tr1::hash
#include <sparsehash/dense_hash_map>
#include "Word.hpp"
#include "city.h"
#include <stdlib.h>
#include <stdio.h>

using google::dense_hash_map;


struct hashstr
{
  std::tr1::hash<string> hasher;
  long operator()(const string& s1) const
  { 
    return CityHash64(s1.data(),s1.size());
  }
};



class WordIndex
{
private:
    dense_hash_map<string, unsigned int,hashstr> map;
public:
  WordIndex()  : map(656538)
  {
     map.set_empty_key("");

  }

  int Put(const std::string& field, const std::string& token, unsigned int value) {
	Word word(field, token);
	return Put(word, value);
  }

  int Get(const std::string& field, const std::string& token, unsigned int& value) {
	Word word(field, token);
    return Get(word, value);
  }


  int Put(const Word& word, unsigned int value) {
	map.insert(make_pair(word.toString(),value));
	return 1;
  }

  int Get(const Word& word, unsigned int& value) {
	auto found = map.find(word.toString());
	if (found != map.end()){
       value = found->second;
       return 1;
    } 
	return 0;		
  }

  ~WordIndex() {
	
  }
};

#endif // WORDINDEX_H
