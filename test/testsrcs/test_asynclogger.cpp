#include "async/Logger.hpp"
#include <unistd.h>

int main(void)
{
	async::Logger::registerFd(STDOUT_FILENO);
	async::Logger::setLogLevel(async::Logger::WARNING);

	async::Logger &root_logger = async::Logger::getLogger("root");

	root_logger << "Hello, World! " << 1 << "\n" << async::debug;
	root_logger << "Hello, World! " << 2 << "\n" << async::verbose;
	root_logger << "Hello, World! " << 3 << "\n" << async::info;
	root_logger << "Hello, World! " << 4 << "\n" << async::warning;
	root_logger << "Hello, World! " << 5 << "\n" << async::error;

	async::Logger::blockingWrite();
	return (0);
}
