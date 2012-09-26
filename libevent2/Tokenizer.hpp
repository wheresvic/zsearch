#include <string>

using namespace std;

class Tokenizer
{
    public:

        Tokenizer(const std::string& s, const std::string& delimiters) :
			m_string(s),
			m_offset(0),
			m_delimiters(delimiters) {}


        bool NextToken()
		{
			return NextToken(m_delimiters);
		}

		bool NextToken(const std::string& delimiters)
		{
			size_t i = m_string.find_first_not_of(delimiters, m_offset);
			if (string::npos == i)
			{
				m_offset = m_string.length();
				return false;
			}

			size_t j = m_string.find_first_of(delimiters, i);
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

        const std::string GetToken() const
        {
			return m_token;
		}

        void Reset()
        {
			m_offset = 0;
			m_string.clear();
			m_token.clear();
			m_delimiters.clear();
		}

    protected:

        std::string m_string;
		size_t m_offset;
        std::string m_delimiters;
        std::string m_token;
};
