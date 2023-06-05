#include "CGI/RequestHandler.hpp"
#include "utils/string.hpp"
#include <cstdlib>
#include <sys/wait.h>
#include <unistd.h>

using namespace CGI;
typedef unsigned long long ull_t;

const size_t RequestHandler::pipeThreshold = 1024;

RequestHandler::RequestHandler(const Request &request,
							   const std::string &exec_path,
							   unsigned int timeout_ms)
	: _request(request), _reader(NULL), _writer(NULL), _exec_path(exec_path),
	  _pid(-1), _waitpid_status(-1), _status(CGI_RESPONSE_INNER_STATUS_BEGIN),
	  _timeout_ms(timeout_ms), _timeout(0),
	  _logger(async::Logger::getLogger("CGIRequestHandler"))
{
}

RequestHandler::~RequestHandler()
{
}

char **RequestHandler::getArgv(void)
{
	char **output = new char *[3];
	output[0] = duplicateStr(_exec_path);
	output[1] = duplicateStr(_request.getPath());
	output[2] = NULL;
	return (output);
}

void RequestHandler::setTimeout(void)
{
	if (_timeout_ms == 0)
	{
		_timeout = 0;
		return;
	}
	_timeout = clock() + ((ull_t)_timeout_ms * (ull_t)(CLOCKS_PER_SEC / 1000));
}

bool RequestHandler::checkTimeout(void)
{
	if (_timeout == 0)
		return (false);
	if (clock() > _timeout)
		throw(std::runtime_error("Timeout occured while executing CGI "
								 + _exec_path));
	return (false);
}

const CGI::Response &RequestHandler::retrieve(void)
{
	if (_status != CGI_RESPONSE_INNER_STATUS_OK)
		throw(std::runtime_error("CGI Response has not been made yet."));
	return (_response);
}

std::string RequestHandler::getMethod(void) const
{
	return (_request.getMethod());
}
