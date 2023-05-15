#include "AsyncIOTaskHandler.hpp"
#include "AsyncSingleIOProcessor.hpp"
#include "ConfigDirective.hpp"
#include "parseConfig.hpp"

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		async::cout << "usage: " << argv[0] << " path/to/config/file\n";
		AsyncIOTaskHandler::task();
		return (2);
	}
	ConfigContext rootConfig;
	try
	{
		rootConfig = parseConfig(argv[1]);
	}
	catch (const std::exception &e)
	{
		async::cerr << e.what() << "\n";
		AsyncIOTaskHandler::task();
		return (2);
	}
	std::cout << "Unimplemented stub of " << __func__ << std::endl;
	return (0);
}
