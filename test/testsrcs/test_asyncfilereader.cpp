#include "async/FileIOProcessor.hpp"
#include <iostream>

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cout << "Usage: " << argv[0] << " file_path" << std::endl;
		return (2);
	}
	try
	{
		async::FileReader reader(0, argv[1]);

		int rc;
		while (true)
		{
			rc = reader.task();
			if (rc == async::FileReader::LOAD_STATUS_OK)
				break;
			else if (rc == async::FileReader::LOAD_STATUS_AGAIN)
				continue;
			else
				throw(std::logic_error("Impossible return code"));
		}
		std::string content = reader.retrieve();
		std::cout << "File content:\n" << content;
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
	}
	return (0);
}
