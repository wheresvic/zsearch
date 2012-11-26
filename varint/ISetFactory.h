#ifndef ISET_FACTORY_H__
#define ISET_FACTORY_H__

#include "Set.h"
#include <memory>

class ISetFactory 
{

	public:
	
		virtual ~ISetFactory()
		{
			
		}
		
		virtual const std::shared_ptr<Set> createSparseSet() = 0;
	
};

#endif
