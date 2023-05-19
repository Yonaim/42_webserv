#include "AsyncLogger.hpp"
#include <unistd.h>

int main(void)
{
	AsyncLogger::registerFd(STDOUT_FILENO);
	AsyncLogger::setLogFilter(AsyncLogger::WARNING);

	AsyncLogger &root_logger = AsyncLogger::getLogger("root");
	AsyncLogger &child_logger = AsyncLogger::getLogger("child");
	child_logger.setLogLevel(AsyncLogger::ERROR);

	root_logger << "Hello, World! " << 1 << async::endl;
	child_logger << "Hello, World! " << 2 << async::endl;

	AsyncLogger::blockingWrite();
	return (0);
}
