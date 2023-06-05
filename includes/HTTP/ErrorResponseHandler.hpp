#ifndef HTTP_ERRORRESPONSEHANDLER_HPP
#define HTTP_ERRORRESPONSEHANDLER_HPP

#include "HTTP/Request.hpp"
#include "HTTP/Response.hpp"
#include "HTTP/Server.hpp"
#include "async/FileIOHandler.hpp"
#include "async/status.hpp"
#include "utils/shared_ptr.hpp"

namespace HTTP
{
class Server::ErrorResponseHandler
{
  private:
	int _request_method;
	Response _response;
	Server *_server;
	int _status;
	int _code;
	unsigned int _timeout_ms;
	ft::shared_ptr<async::FileReader> _reader;
	async::Logger &_logger;

	void generateDefaultErrorResponse(void);
	void generateResponse(const std::string &body);

  public:
	enum response_status_e
	{
		RESPONSE_STATUS_OK = 0,
		RESPONSE_STATUS_AGAIN
	};

	ErrorResponseHandler(Server *server, const int request_method,
						 const int code, const unsigned int timeout_ms);
	~ErrorResponseHandler();

	int task(void);
	Response retrieve(void);
};
} // namespace HTTP

#endif
