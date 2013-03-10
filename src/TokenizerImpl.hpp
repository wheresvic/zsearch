
#ifndef TOKENIZERIMPL_H
#define TOKENIZERIMPL_H

#include <string>
#include "ITokenizer.h"
#include <iostream>
#include <cctype>
#include <climits>
using namespace std;
class TokenizerImpl : public ITokenizer
{
	protected:
        bool table[UCHAR_MAX+1];
    public:

		TokenizerImpl(): m_offset(0)
		{
			for (int i = 0; i <= UCHAR_MAX; ++i)
			    table[i] = std::isalnum(i);
		}
		
		~TokenizerImpl(){ 
			std::cerr << "Destroyed TokenizerImpl" << std::endl;
		}
		
		void setString(const std::string& str,const std::string& field)
		{
			m_string = str;
			stringsize = m_string.size();
			m_token.clear();
			m_offset = 0;
		}

        inline bool nextToken()
		{
			m_token.resize(0);
			size_t tokensize = 0;
			for (;;){
				if (m_offset == stringsize) break; //22%
				unsigned char c = m_string[m_offset++]; 
				c = c | 0x20;
				if (table[c]) // 39%
				{
					tokensize++;
					m_token.push_back(c);
				} else {
					if (tokensize>2){
					  return true;
					} else {
					  m_token.resize(0);	
					}
				}
			}
			return tokensize>2;
		}
		
        
		inline const std::string& getToken() const
		{	
			return m_token;
		}

	protected:

        std::string m_string;
		size_t stringsize;
		size_t m_offset;
        std::string m_token;


};

#endif
