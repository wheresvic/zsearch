
#ifndef ITOKENIZER_H
#define ITOKENIZER_H

#include <string>

class ITokenizer
{
    public:

        virtual bool nextToken()  = 0;

        virtual const std::string& getToken() const = 0;

		virtual void setString(const std::string& str,const std::string& field) = 0;

		virtual ~ITokenizer() { }
};

#endif
