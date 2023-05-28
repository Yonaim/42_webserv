#include "CGI/RequestHandler.hpp"
#include <iostream>
#include <unistd.h>

int main()
{
	async::Logger::registerFd(STDOUT_FILENO);
	async::Logger::setLogLevel("DEBUG");
	try
	{
		CGI::Request cgi_request;
		CGI::RequestHandler cgi_request_handler(cgi_request, 1000);
		async::Logger::blockingWrite();
		while (1)
		{
			int rc = cgi_request_handler.task();
			async::Logger::blockingWrite();

			if (rc == CGI::RequestHandler::CGI_RESPONSE_STATUS_OK)
				break;
		    else
				continue;
		}
	}
	catch (std::exception &e)
	{
		std::cout << e.what() << std::endl;
	}
}
