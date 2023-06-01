#include "CGI/RequestHandler.hpp"
#include "utils/string.hpp"
#include <cstdlib>
#include <sys/wait.h>
#include <unistd.h>

using namespace CGI;

const size_t RequestHandler::pipeThreshold = 1024;

RequestHandler::RequestHandler(const Request &request,
							   const std::string &exec_path)
	: _request(request), _reader(NULL), _writer(NULL), _exec_path(exec_path),
	  _pid(-1), _waitpid_status(-1), _status(CGI_RESPONSE_INNER_STATUS_BEGIN),
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

const CGI::Response &RequestHandler::retrieve(void)
{
	if (_status != CGI_RESPONSE_INNER_STATUS_OK)
		throw(std::runtime_error("CGI Response has not been made yet."));
	return (_response);
}
