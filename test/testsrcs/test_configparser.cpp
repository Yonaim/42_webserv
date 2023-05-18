#include "parseConfig.hpp"
#include <iostream>

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cout << "Usage: " << argv[0] << " config_file_path" << std::endl;
		return (2);
	}
	try
	{
		const ConfigContext root = parseConfig(argv[1]);
		std::cout << root << "\n";
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
	}
	return (0);
}
