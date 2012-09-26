#include <string>
#include <iostream>
#include <cstdio>

std::string decode(const std::string &input)
{
    std::string decoded;

    char ch;
    int i, ii;

    for (i=0; i < input.length(); i++)
    {
		if (input[i] == '%')
		{
            std::sscanf(input.substr(i+1, 2).c_str(), "%x", &ii);
            ch = static_cast<char>(ii);
            decoded += ch;
            i = i+2;
        }
        else
        {
            decoded += input[i];
        }
    }

    return decoded;
}

int main()
{

	std::string input;

	while (std::cin >> input)
	{
		std::cout << decode(input) << std::endl;
	}

	return 0;
}
