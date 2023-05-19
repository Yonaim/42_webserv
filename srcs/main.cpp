#include "AsyncIOTaskHandler.hpp"
#include "AsyncSingleIOProcessor.hpp"
#include "ConfigDirective.hpp"
#include "WebServer.hpp"
#include "parseConfig.hpp"
#include <iostream>

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cout << "usage: " << argv[0] << " path/to/config/file\n";
		return (2);
	}

	ConfigContext rootConfig;
	try
	{
		rootConfig = parseConfig(argv[1]);
	}
	catch (const std::exception &e)
	{
		std::cerr << "Error while parsing config file: " << e.what() << "\n";
		return (2);
	}

	try
	{
		WebServer webserver(rootConfig);
		while (true)
			webserver.task();
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
	}

	return (0);
}
