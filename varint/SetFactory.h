#ifndef SET_FACTORY_H__
#define SET_FACTORY_H__
#include "Set.h"
#include "CompressedSet.h"
#include "BasicSet.h"
#include <memory>

enum SetType
{ 
	CompressedSet_t,
	BasicSet_t
};


class SetFactory 
{
public:

	SetFactory()
	{
		
	}
	
	~SetFactory()
	{
		
	}
	
	/*
	virtual const shared_ptr<Set> createSparseSet()
	{
		return make_shared<CompressedSet>();
	}
	*/
	
	virtual const shared_ptr<Set> createSet(const SetType setType)
	{
		if (setType == CompressedSet_t)
			return std::make_shared<CompressedSet>();
		
		return make_shared<BasicSet>();
	}
	
};

#endif //SET_FACTORY_H__