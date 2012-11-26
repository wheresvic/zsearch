#ifndef BASIC_SET_FACTORY_H__
#define BASIC_SET_FACTORY_H__

#include "Set.h"
#include "BasicSet.h"
#include "ISetFactory.h"
#include <memory>

class BasicSetFactory : public ISetFactory 
{

	public:
	
		virtual const shared_ptr<Set> createSparseSet()
		{
			return make_shared<BasicSet>();
		}
	
};

#endif 
