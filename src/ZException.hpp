
/**
 * http://stackoverflow.com/questions/134569/c-exception-throwing-stdstring
 */
 
#ifndef ZEXCEPTION_H
#define ZEXCEPTION_H

#include <string>

struct ZException : public std::exception
{
	std::string s;
	
	ZException(std::string ss) : s(ss) { }
	
	const char* what() const throw() 
	{ 
		return s.c_str(); 
	}
};

#endif
