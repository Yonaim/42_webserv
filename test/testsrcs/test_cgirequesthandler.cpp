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

		cgi_request.setMessageBody("this is the body man~");

		cgi_request.setMetaVariable("CONTENT_LENGTH", "10");
		cgi_request.setMetaVariable("CONTENT_TYPE", "text/html");
		cgi_request.setMetaVariable(
			"PATH_INFO", "./test/testcase/cgirequesthandler/cgi_program.exe");
		cgi_request.setMetaVariable(
			"PATH_TRANSLATED", "/Users/hyeyukim/42cursus/webserv/test/testcase/"
							   "cgirequesthandler/cgi_program.exe");
		cgi_request.setMetaVariable("QUERY_STRING", "hi hi");
		cgi_request.setMetaVariable("REQUEST_METHOD", "GET");
		cgi_request.setMetaVariable("SERVER_NAME", "localhost");
		cgi_request.setMetaVariable("SERVER_PORT", "80");

		CGI::RequestHandler cgi_request_handler(cgi_request, 1000);
		async::Logger::blockingWrite();
		while (1)
		{
			int rc = cgi_request_handler.task();
			async::Logger::blockingWrite();
			sleep(1);

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
