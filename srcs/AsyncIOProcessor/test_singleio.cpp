#include "AsyncSingleIOProcessor.hpp"

int main(void)
{
	async::cout << "Hello, World!\n";
	std::string buf;
	async::cin >> buf;
	async::cout << "Echo: " << buf;
	async::cout.task();
	async::cin.task();
	return (0);
}
