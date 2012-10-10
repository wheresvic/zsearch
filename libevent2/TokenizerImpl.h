
#ifndef TOKENIZERIMPL_H
#define TOKENIZERIMPL_H

#include <string>
#include "ITokenizer.h"

class TokenizerImpl : public ITokenizer
{
    public:

		TokenizerImpl(const std::string& delimiters);

		void setString(const std::string& str);

        bool nextToken();

        const std::string getToken() const;

	protected:

        std::string m_string;
		size_t m_offset;
        const std::string m_delimiters;
        std::string m_token;


};

#endif
