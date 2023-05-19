#include "async/IOTaskHandler.hpp"
#include "async/SingleIOProcessor.hpp"
#include <unistd.h>

int main(void)
{
	async::cout << "Hello, World!\nSleeping for 5 secs: ";
	async::IOTaskHandler::task();
	sleep(5);
	async::IOTaskHandler::task();
	std::string buf;
	async::cin >> buf;
	async::cout << "Echo to STDOUT: " << buf;
	async::cerr << "Echo to STDERR: " << buf;
	async::IOTaskHandler::task();
	return (0);
}
