
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

using namespace std;

class DocumentImpl : public IDocument
{
	public:

		DocumentImpl()
		{

		}

		DocumentImpl(const string& xml)
		{
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

		void write(ostream& out)
		{
			// for some weird reason rapidxml_print is not working ...
			// either fix or consider using another xml library

			/*
			string document;

			rapidxml::xml_document<> doc;
			rapidxml::xml_node<> *root = doc.allocate_node(rapidxml::node_element, "document");
			doc.append_node(root);

			for (auto it = entries.begin(); it != entries.end(); ++it)
			{
				rapidxml::xml_node<> *node = doc.allocate_node(rapidxml::node_element, (it->first).c_str(), (it->second).c_str());
				root->append_node(node);
			}
			*/

			// rapidxml::print(std::back_inserter(document), doc);
			// out << document;

			/*
			for (rapidxml::xml_node<>* n = doc.first_node("document")->first_node(); n; n = n->next_sibling())
			{
				string field(n->name());
				string value(n->value());
				std::cout << field << " " << value << std::endl;
			}
			*/

			out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
			out << "<document>";

			for (auto it = entries.begin(); it != entries.end(); ++it)
			{
				out << "<" << it->first << ">";
				out << encodeForXml(it->second);
				out << "</" << it->first << ">";
			}

			out << "</document>";

		}

	private:

		map<string, string> entries;

		std::string encodeForXml( const std::string &sSrc )
		{
			ostringstream sRet;

			for( string::const_iterator iter = sSrc.begin(); iter!=sSrc.end(); iter++ )
			{
				unsigned char c = (unsigned char)*iter;

				switch(c)
				{
					case '&': sRet << "&amp;"; break;
					case '<': sRet << "&lt;"; break;
					case '>': sRet << "&gt;"; break;
					case '"': sRet << "&quot;"; break;
					case '\'': sRet << "&apos;"; break;

					default:
						sRet << c; break;

						/*
						if ( c<32 || c>127 )
						{
							sRet << "&#" << (unsigned int)c << ";";
						}
						else
						{
							sRet << c;
						}
						*/
				}
			}

			return sRet.str();
		}

};

#endif
