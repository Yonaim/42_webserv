#include "async/Logger.hpp"
#include <unistd.h>

int main(void)
{
	async::Logger::registerFd(STDOUT_FILENO);
	async::Logger::setLogLevel(async::Logger::DEBUG);

	async::Logger &root_logger = async::Logger::getLogger("root");

	root_logger << async::debug << "Hello, World! " << 1;
	root_logger << async::verbose << "Hello, World! " << 2;
	root_logger << async::info << "Hello, World! " << 3;
	root_logger << async::warning << "Hello, World! " << 4;
	root_logger << async::error << "Hello, World! " << 5;

	async::Logger::blockingWrite();
	return (0);
}
