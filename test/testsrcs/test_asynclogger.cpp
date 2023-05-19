#include "AsyncLogger.hpp"
#include <unistd.h>

int main(void)
{
	AsyncLogger::registerFd(STDOUT_FILENO);
	AsyncLogger::setLogLevel(AsyncLogger::WARNING);

	AsyncLogger &root_logger = AsyncLogger::getLogger("root");

	root_logger << "Hello, World! " << 1 << "\n" << async::debug;
	root_logger << "Hello, World! " << 2 << "\n" << async::verbose;
	root_logger << "Hello, World! " << 3 << "\n" << async::info;
	root_logger << "Hello, World! " << 4 << "\n" << async::warning;
	root_logger << "Hello, World! " << 5 << "\n" << async::error;

	AsyncLogger::blockingWrite();
	return (0);
}
