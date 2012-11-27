
#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <string>

// #define QUERY_PARSER_DELIMITERS " \t\n\r.,"
// #define POST_DATA_KEY "data"

namespace zsearch
{
	const std::string QUERY_PARSER_DELIMITERS = " \t\n\r.,;";
	//const char KEYWORD_SPLITTER = '/';

	const std::string POST_DATA_KEY = "data";
	const std::string GET_SEARCH_QUERY_KEY = "q";
	const std::string GET_SEARCH_START_KEY = "s";
	const std::string GET_SEARCH_OFFSET_KEY = "o";
	const std::string DOC_ID_KEY = "id";

	const std::string DOCUMENT_ROOT = "document";
	const std::string DOCUMENT_TITLE = "title";
}


#endif
