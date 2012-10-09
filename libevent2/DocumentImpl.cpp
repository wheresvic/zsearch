
#include <map>
#include <string>
#include <utility>
#include <iterator>
#include <vector>
#include <iostream>

#include "rapidxml.hpp"
#include "DocumentImpl.h"
#include "Constants.hpp"

using namespace std;


DocumentImpl::DocumentImpl() : title("Document")
{ }

DocumentImpl::DocumentImpl(const string& xml)
{
	vector<char> xmlVec;
	copy (xml.begin(), xml.end(), back_inserter(xmlVec));
	xmlVec.push_back('\0');

	rapidxml::xml_document<> doc;    					// character type defaults to char
	doc.parse<rapidxml::parse_full>(&xmlVec[0]);    	// 0 means default parse flags

	string message = "Invalid document, please see documentation for sample document xml";

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

				if (field.compare(zsearch::DOCUMENT_TITLE) == 0)
				{
					this->setTitle(value);
				}
				else
				{
					this->addEntry(field, value);
				}
			}

			return;
		}
	}

	throw message;
}

void DocumentImpl::setTitle(const string& title)
{
	this->title = title;
}

const string& DocumentImpl::getTitle()
{
	return title;
}

void DocumentImpl::addEntry(const string& key, const string& value)
{
	entries.insert(make_pair(key, value));
}

const map<string, string>& DocumentImpl::getEntries()
{
	return entries;
}


