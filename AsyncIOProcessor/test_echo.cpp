#include "AsyncTCPIOProcessor.hpp"
#include <iostream>

int main(void)
{
	AsyncTCPIOProcessor listener;
	while (true)
	{
		listener.task();
		for (AsyncTCPIOProcessor::iterator it = listener.begin();
			 it != listener.end(); it++)
		{
			int fd = *it;
			if (listener.rdbuf(fd).length() == 0)
				continue;
			std::cout << "Write to buf of " << fd << ":\"" << listener.rdbuf(fd)
					  << "\"" << std::endl;
			listener.wrbuf(fd) = listener.wrbuf(fd) + listener.rdbuf(fd);
			listener.rdbuf(fd) = "";
		}
	}
	return (0);
}
