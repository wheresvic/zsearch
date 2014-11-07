
#ifndef TOKENIZERIMPL_H
#define TOKENIZERIMPL_H

#include <string>
#include "ITokenizer.h"

class TokenizerImpl : public ITokenizer
{
    public:

		TokenizerImpl();

		~TokenizerImpl();
		
		void setString(const std::string& str,const std::string& field);

        bool nextToken();
		
        const std::string& getToken() const;

	protected:

        std::string m_string;
		size_t m_offset;
        std::string m_token;


};

#endif
