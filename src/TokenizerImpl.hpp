
#ifndef TOKENIZERIMPL_H
#define TOKENIZERIMPL_H

#include <string>
#include "ITokenizer.h"
#include <iostream>
using namespace std;
class TokenizerImpl : public ITokenizer
{
    public:

		TokenizerImpl(): m_offset(0)
		{
			
		}
		
		~TokenizerImpl(){ 
			std::cerr << "Destroyed TokenizerImpl" << std::endl;
		}
		
		void setString(const std::string& str)
		{
			m_string = str;
			stringsize = m_string.size();
			m_token.clear();
			m_offset = 0;
		}

        inline bool nextToken()
		{
			m_token.resize(0);
			for (;;){
				if (m_offset == stringsize) break; //22%
				char c = m_string[m_offset++];
				c = c | 0x20;	// lowercase
				if (c >= 'a' ? c <= 'z' : (c >= '0' && c <= '9')) // 39%
				{
					m_token.push_back(c);
				} else {
					if (m_token.size()>0)
					  return true;
				}
			}
			return m_token.size()>0;
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
