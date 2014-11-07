
#include <iostream>
#include <string>
#include <algorithm>
#include <locale>
#include "TokenizerImpl.h"

using namespace std;

TokenizerImpl::TokenizerImpl() : m_offset(0)
{ }

TokenizerImpl::~TokenizerImpl()
{ 
	std::cerr << "Destroyed TokenizerImpl" << std::endl;
}


void TokenizerImpl::setString(const std::string& str, const std::string& field)
{
	m_string = str;
	m_token.clear();
	m_offset = 0;
}


bool TokenizerImpl::nextToken()
{
	m_token.resize(0);
	for (;;){
		if (m_offset == m_string.size()) break;
		char c = m_string[m_offset++];
		c = c | 0x20;	// lowercase
		if (c >= 'a' ? c <= 'z' : (c >= '0' && c <= '9'))
		{
			m_token.push_back(c);
		} else {
			if (m_token.size()>0)
			  return true;
		}
	}
	return m_token.size()>0;
}



const std::string& TokenizerImpl::getToken() const
{	
	return m_token;
}
