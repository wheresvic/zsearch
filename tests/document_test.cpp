
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <memory>
#include <exception>
#include <iostream>
#include <iterator>

#include "DocumentImplTest.hpp"

#include "lib/tpunit++.hpp"

using namespace std;

int main()
{
	// QueryParserTest __QueryParserTest;
	// XmlTest __XmlTest;
	DocumentImplTest __DocumentImplTest;
	// InvertedIndexSimpleTest __InvertedIndexSimpleTest;

	/**
	 * Run all of the registered tpunit++ tests. Returns 0 if
	 * all tests are successful, otherwise returns the number
	 * of failing assertions.
	 */
	return tpunit::Tests::Run();

}
