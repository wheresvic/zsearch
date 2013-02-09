
#ifndef WORD_H
#define WORD_H

#include <string>
#include "Constants.hpp"

using namespace std;

class Word
{
	public:

		Word(const string& field, const string& word) : field(field), word(word)
		{
		}

		/*
		Word(const string& field, const string&& word) : field(field), word(std::move(word))
		{
		}
		*/

		const string& getField() const
		{
			return field;
		}

		const string& getWord() const
		{
			return word;
		}

		bool operator<(const Word& compare) const
		{
			string me = field + '/' + word;
			string other = compare.getField() + '/' + compare.getWord();
			return me < other;
		}

		const string toString() const {
			return field + '/' + word;
		}


	private:
		string field;
		string word;
};

#endif
