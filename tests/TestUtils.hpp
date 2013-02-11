
#ifndef TESTUTILS_HPP
#define TESTUTILS_HPP

#include <string>
#include <exception>
#include <fstream>

using namespace std;

string readFile(const string& fileName)
{
	ifstream fin(fileName.c_str());

	if (fin.fail())
		throw "Could not open " + fileName + "!";

	fin.seekg(0, ios::end);
	size_t length = fin.tellg();
	fin.seekg(0, ios::beg);
	char* buffer = new char[length + 1];
	fin.read(buffer, length);
	buffer[length] = '\0';

	fin.close();

	string fileStr(buffer);
	delete [] buffer;

	return fileStr;
}

string stripSpecialCharacters(const string& input)
{
	string clean;

	for (char c : input)
	{
		if (c == '\n' || c == '\t')
			continue;

		clean += c;
	}

	return clean;
}

#endif
