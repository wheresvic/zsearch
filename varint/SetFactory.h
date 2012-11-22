#ifndef SET_FACTORY_H__
#define SET_FACTORY_H__
#include "Set.h"
#include "CompressedSet.h"
#include <memory>
class SetFactory {
public:
	SetFactory(){
		
	}
	
	~SetFactory(){
		
	}
	
	virtual const shared_ptr<Set> createSparseSet(){
		return make_shared<CompressedSet>();
	}
	
};

#endif //SET_FACTORY_H__
