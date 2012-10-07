
using namespace std;

#include "varint/CompressedSet.h"
#include <iostream>
#include <string>

int main()
{
	CompressedSet set;
	set.addDoc(2);
	set.addDoc(3);
	set.addDoc(2);

	CompressedSet::Iterator it(&set);

	for (; it.docID() != NO_MORE_DOCS; it.nextDoc())
	{
		cout << it.docID() << " ";
	}

	return 0;
}
