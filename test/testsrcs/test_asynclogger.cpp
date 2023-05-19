#include "AsyncLogger.hpp"
#include <unistd.h>

int main(void)
{
	AsyncLogger::registerFd(STDOUT_FILENO);
	AsyncLogger::setLogFilter(AsyncLogger::WARNING);

	AsyncLogger &root_logger = AsyncLogger::getLogger("root");
	AsyncLogger::setLogLevel(AsyncLogger::ERROR);
	AsyncLogger &child_logger = AsyncLogger::getLogger("child");

	root_logger << "Hello, World! " << 1 << async::endl;
	child_logger << "Hello, World! " << 2 << async::endl;

	AsyncLogger::task();
	return (0);
}
