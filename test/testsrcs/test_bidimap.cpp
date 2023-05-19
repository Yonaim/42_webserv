#include "BidiMap.hpp"
#include <iostream>
#include <string>

int main()
{
	BidiMap<int, std::string> bidiMap;

	bidiMap.insert(1, "one");
	bidiMap.insert(2, "two");
	bidiMap.insert(3, "three");
	bidiMap.insert(5, "five");

	try
	{
		std::cout << bidiMap.getValueByKey(1) << '\n';
		std::cout << bidiMap[2] << '\n';
		std::cout << bidiMap.getKeyByValue("three") << '\n';
		// std::cout << bidiMap.getKeyByValue("four") << '\n';
		std::cout << bidiMap["five"] << '\n';
		std::cout.flush();
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
	}
}
