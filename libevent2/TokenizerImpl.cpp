
#include <string>
#include "TokenizerImpl.h"

using namespace std;

TokenizerImpl::TokenizerImpl(const std::string& delimiters) : m_offset(0), m_delimiters(delimiters)
{ }

void TokenizerImpl::setString(const std::string& str)
{
	m_string = str;
	m_token.clear();
	m_offset = 0;
}

bool TokenizerImpl::nextToken()
{
	size_t i = m_string.find_first_not_of(m_delimiters, m_offset);

	if (string::npos == i)
	{
		m_offset = m_string.length();
		return false;
	}

	size_t j = m_string.find_first_of(m_delimiters, i);
	if (string::npos == j)
	{
		m_token = m_string.substr(i);
		m_offset = m_string.length();
		return true;
	}

	m_token = m_string.substr(i, j - i);
	m_offset = j;
	return true;
}

const std::string TokenizerImpl::getToken() const
{
	return m_token;
}
