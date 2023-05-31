#include "async/SingleIOProcessor.hpp"
#include <unistd.h>

int main(void)
{
	async::cout << "Hello, World!\nSleeping for 5 secs: ";
	async::IOProcessor::doAllTasks();
	sleep(5);
	async::IOProcessor::doAllTasks();
	std::string buf;
	async::cin >> buf;
	async::cout << "Echo to STDOUT: " << buf;
	async::cerr << "Echo to STDERR: " << buf;
	async::IOProcessor::doAllTasks();
	return (0);
}
