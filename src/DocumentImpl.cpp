
#include <map>
#include <string>
// #include <utility>
#include <iterator>
#include <vector>
#include <iostream>

#include <rapidxml.hpp>
#include "DocumentImpl.h"
#include "Constants.hpp"

using namespace std;

DocumentImpl::DocumentImpl()
{

}

DocumentImpl::DocumentImpl(const string& xml)
{
	vector<char> xmlVec;
	copy (xml.begin(), xml.end(), back_inserter(xmlVec));
	xmlVec.push_back('\0');

	string message = "Invalid document, please see documentation for sample document xml";

	try
	{
		rapidxml::xml_document<> doc;    					// character type defaults to char
		doc.parse<rapidxml::parse_full>(&xmlVec[0]);    	// 0 means default parse flags

		if (doc.first_node())
		{
			string rootNode(doc.first_node()->name());

			if (rootNode.compare(zsearch::DOCUMENT_ROOT) == 0)
			{

				for (rapidxml::xml_node<>* n = doc.first_node(rootNode.c_str())->first_node(); n; n = n->next_sibling())
				{
					string field(n->name());

					char* v = n->value();

					if (!v || !*v)
					{
						message = "Invalid document: " + field + " has empty string";
						throw message;
					}

					string value(v);
					this->addEntry(field, value);
				}

				return;
			}
		}
	}
	catch (const rapidxml::parse_error& e)
	{
		string error(e.what());
		throw error;
	}

	throw message;
}

void DocumentImpl::addEntry(const string& key, const string& value)
{
	entries.insert(make_pair(key, value));
}

const map<string, string>& DocumentImpl::getEntries()
{
	return entries;
}

void DocumentImpl::getEntry(const string& key, string& value)
{
	if (entries.find(key) != entries.end()) {
		value = entries[key];
	}

	return;
}

void DocumentImpl::write(ostream& out)
{
	out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
	out << "<document>";

	for (auto it = entries.begin(); it != entries.end(); ++it)
	{
		out << "<" << it->first << ">";
		out << it->second;
		out << "</" << it->first << ">";
	}

	out << "</document>";
}

