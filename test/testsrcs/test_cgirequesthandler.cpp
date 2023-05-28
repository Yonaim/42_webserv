#include "CGI/RequestHandler.hpp"
#include <iostream>

int main()
{
	try{
	CGI::Request cgi_request;
	CGI::RequestHandler cgi_request_handler(cgi_request, 1000);

    while (1)
    {
		int rc = cgi_request_handler.task();

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
