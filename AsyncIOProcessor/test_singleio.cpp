#include "AsyncIOTaskHandler.hpp"
#include "AsyncSingleIOProcessor.hpp"
#include <unistd.h>

int main(void)
{
	async::cout << "Hello, World!\nSleeping for 5 secs: ";
	AsyncIOTaskHandler::task();
	sleep(5);
	AsyncIOTaskHandler::task();
	std::string buf;
	async::cin >> buf;
	async::cout << "Echo to STDOUT: " << buf;
	async::cerr << "Echo to STDERR: " << buf;
	AsyncIOTaskHandler::task();
	return (0);
}
