#include "BidiMap.hpp"
#include <iostream>
#include <string>
#include <map>

int main()
{
	BidiMap<int, std::string> bidiMap1;

	bidiMap1.insert(1, "one");
	bidiMap1.insert(2, "two");
	bidiMap1.insert(3, "three");
	bidiMap1.insert(5, "five");

	try
	{
		std::cout << bidiMap1.getValueByKey(1) << '\n';
		std::cout << bidiMap1[2] << '\n';
		std::cout << bidiMap1.getKeyByValue("three") << '\n';
		// std::cout << bidiMap1.getKeyByValue("four") << '\n';
		std::cout << bidiMap1["five"] << '\n';
		std::cout.flush();
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
	}

	std::map<int, std::string> map;
	map.insert(std::pair<int, std::string>(123, "test1"));
	map.insert(std::pair<int, std::string>(456, "test2"));
	map.insert(std::pair<int, std::string>(789, "test3"));
	BidiMap<int, std::string> bidiMap2(map);

	std::cout << bidiMap2.getValueByKey(123) << '\n';
	std::cout << bidiMap2.getValueByKey(456) << '\n';
	std::cout << bidiMap2.getValueByKey(789) << '\n';
	std::cout.flush();
}
