#include "CGI/RequestHandler.hpp"
#include "async/IOTaskHandler.hpp"
#include "async/SingleIOProcessor.hpp"
#include <iostream>
#include <unistd.h>

int main()
{
	async::Logger::registerFd(STDOUT_FILENO);
	async::Logger::setLogLevel("DEBUG");
	try
	{
		std::string buffer
			= "POST /simple.cgi HTTP/1.1\r\n"
			  "User-Agent: Mozilla/ 4.0(compatible; MSIE5 .01; Windows NT)\r\n"
			  "Host: localhost\r\n"
			  "Accept-Language: en-us\r\n"
			  "Accept-Encoding: gzip, deflate\r\n"
			  "Connection: Keep-Alive\r\n"
			  "Content-Length: 10\r\n"
			  "\r\n"
			  "aaaaaaaaaa";

		HTTP::Request http_request;
		http_request.parse(buffer, 100000);

		CGI::Request cgi_request(http_request,
								 "./test/testcase/cgi_script/simple_cgi.py",
								 "what the heck");
		CGI::RequestHandler cgi_request_handler(
			cgi_request, "./test/testcase/cgi_script/simple_cgi.py", 10000);
		async::Logger::blockingWrite();

		while (1)
		{
			async::IOTaskHandler::task();
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
		async::Logger::blockingWrite();
	}
}
