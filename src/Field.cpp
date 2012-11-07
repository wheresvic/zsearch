#include "Field.h"

Field::Field(const char* name, const char* value,int _config){
	//assert(name != NULL);
    _name = name;
	fieldsData = (void*)value;
	setConfig(_config);
}

void Field::setConfig(const uint32_t x){
	uint32_t newConfig=0;

    if ( x & STORE_YES ){
	  newConfig |= STORE_YES;
    } else {
	  newConfig |= STORE_NO;
    }

    if ( (x & INDEX_NO)==0 ){
      bool index=false;	  
      if ( x & INDEX_TOKENIZED ){
	    newConfig |= INDEX_TOKENIZED;
		index = true;
      } else if ( x & INDEX_UNTOKENIZED ){
	    newConfig |= INDEX_UNTOKENIZED;
	    index = true;
	  }
	  if ( !index )
	    newConfig |= INDEX_NO;
    } else {
	  newConfig |= INDEX_NO;
    }

    if ( newConfig & INDEX_NO && newConfig & STORE_NO ){
	    // it doesn't make sense to have a field that is neither indexed nor stored
    }
    config = newConfig;
}

const char* Field::name() const {
	return _name; 
}

const char* Field::value() const {
	return static_cast<char*>(fieldsData);
}

bool Field::isStored() const {
  return (config & STORE_YES) != 0;
}

bool Field::isIndexed() const {
  return (config & INDEX_TOKENIZED)!=0 ; 
}

bool Field::isTokenized() const {
  return (config & INDEX_TOKENIZED) != 0;
}

const char* Field::toString(){
	return static_cast<char*>(fieldsData);	
}

Field::~Field() {
	//TODO
}
