
#ifndef DOCUMENTIMPL_H
#define DOCUMENTIMPL_H

#include <map>
#include <exception>
#include <string>
#include <memory>

#include <sstream>
#include <iterator>
#include <vector>
#include <iostream>


#include <rapidxml.hpp>
#include <rapidxml_print.hpp>

#include "Constants.hpp"
#include "IDocument.h"
#include "ZUtil.hpp"

using namespace std;

class DocumentImpl : public IDocument
{
	public:

		DocumentImpl()
		{

		}

		void construct(const string& xml)
		{
			entries.clear();

			vector<char> xmlVec;
			copy (xml.begin(), xml.end(), back_inserter(xmlVec));
			xmlVec.push_back('\0');

			string message = "Invalid document, please see documentation for sample document xml";

			try
			{
				rapidxml::xml_document<> doc;    					// character type defaults to char
				doc.parse<rapidxml::parse_default>(&xmlVec[0]);    	// 0 means default parse flags

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

		DocumentImpl(const string& xml)
		{
			construct(xml);
		}

		void addEntry(const string& key, const string& value)
		{
			entries.insert(make_pair(key, value));
		}

		const map<string, string>& getEntries()
		{
			return entries;
		}

		void getEntry(const string& key, string& value)
		{
			if (entries.find(key) != entries.end()) {
				value = entries[key];
			}
		}
		
		void readMini(const string& src){
			entries.clear();
			leveldb::Slice s(src);
			uint32_t numelem=0;
			ZUtil::GetVarint32(&s,&numelem);
			for (unsigned int i = 0; i<= numelem; i++){
				leveldb::Slice key;
				ZUtil::GetLengthPrefixedSlice(&s,&key);
				leveldb::Slice value;
				ZUtil::GetLengthPrefixedSlice(&s,&value);
				this->addEntry(key.ToString(), value.ToString());
			}
		}

        void writeMini(ostream& out){
			std::string dst;
			unsigned int size = entries.size();
			ZUtil::PutVarint32(&dst,size);
			for (auto it = entries.begin(); it != entries.end(); ++it)
			{
				const std::string& key = it->first;
				ZUtil::PutLengthPrefixedSlice(&dst,key);
				const std::string& value = it->second;
				ZUtil::PutLengthPrefixedSlice(&dst,value);
			}
			out << dst;
        }

		void write(ostream& out)
		{
			string document;

			rapidxml::xml_document<> doc;

			rapidxml::xml_node<>* decl = doc.allocate_node(rapidxml::node_declaration);
			decl->append_attribute(doc.allocate_attribute("version", "1.0"));
			decl->append_attribute(doc.allocate_attribute("encoding", "utf-8"));
			doc.append_node(decl);

			rapidxml::xml_node<> *root = doc.allocate_node(rapidxml::node_element, "document");
			doc.append_node(root);

			for (auto it = entries.begin(); it != entries.end(); ++it)
			{
				rapidxml::xml_node<> *node = doc.allocate_node(rapidxml::node_element, (it->first).c_str(), (it->second).c_str());
				root->append_node(node);
			}

			rapidxml::print(std::back_inserter(document), doc);
			out << document;
		}

	private:

		map<string, string> entries;

};

#endif
