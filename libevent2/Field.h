/**
A field is a section of a Document. 
Each field has two parts, a name and a value. 
Values may be free text, or an atomic keywords, which are not further processed.
Such keywords maybe used to represent dates, urls, etc.
Fields are optionally stored so that they may be returned the document.
*/
#include <cstddef>
#include <stdint.h> 

class Field {
public:
	enum Store{ 
		/** Store the original field value. 
		* This is useful for short texts like doc title
		* which should be displayed with the results. 
		*/
		STORE_YES=1,
		
	    /** Do not store the field value in the index. */
		STORE_NO=2
	};
	
	enum Index{ 
		/** Do not index the field value.
		* This field can thus not be searched,
		* but one can still access its contents provided it is stored
		*/
		INDEX_NO=16,
		
		/** Index the field's value so it can be searched.
		 * An Analyzer will be used to tokenize the text before its
		 * terms will be stored in the index.
		 */
		INDEX_TOKENIZED=32,
		
		/** Index the field's value without using an Analyzer,
		* so it can be searched.
		* The value will be stored as a single term. 
		* This is useful for unique Ids like product numbers.
		*/
		INDEX_UNTOKENIZED=64 
	};
	
	Field(const char* name, const char* value,int _config);
	
	virtual ~Field();
	
	virtual const char* name() const;
	virtual const char* value() const;
	virtual bool isStored() const;
	virtual bool isIndexed() const;
	virtual bool isTokenized() const;
	/** Prints a Field for human consumption. */
	virtual const char* toString();
	
protected:	
	void setConfig(const uint32_t _config);
	const char* _name;
	void* fieldsData;
	uint32_t config;
};