
#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <string>

namespace zsearch
{
	const std::string LOCK_FILE = "/var/tmp/zsearch.pid";

	const std::string LEVELDB_STORE = "/var/tmp/store";
	const std::string LEVELDB_TEST_STORE = "/var/tmp/test_store";

	const std::string FIELDS_KEY = "fields";
	const std::string DOC_ID_KEY = "docId";
	const std::string WORD_ID_KEY = "wordId";

	// const std::string QUERY_PARSER_DELIMITERS = " \t\n\r.,;";

	namespace server
	{
		const unsigned short PORT = 8080;

		const std::string POST_HTM = "/post.htm";
		const std::string SEARCH_PATH = "/search";
		const std::string POST_PATH = "/post";
		const std::string DOC_PATH = "/doc";
		const std::string INDEX_PATH = "/index";
		const std::string ROOT = "/";

		const std::string POST_DATA_KEY = "data";
		const std::string GET_SEARCH_QUERY_KEY = "q";
		const std::string GET_SEARCH_START_KEY = "s";
		const std::string GET_SEARCH_OFFSET_KEY = "o";
		const std::string DOC_ID_KEY = "id";
	}

	const unsigned int MAX_BATCH_SIZE = 200;

	const std::string DOCUMENT_ROOT = "document";
	const std::string DOCUMENT_TITLE = "title";
}


#endif
