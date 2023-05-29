#ifndef HTTP_REQUESTHANDLER_HPP
#define HTTP_REQUESTHANDLER_HPP

#include "CGI/RequestHandler.hpp"
#include "HTTP/Request.hpp"
#include "HTTP/Response.hpp"
#include "HTTP/Server.hpp"
#include "async/FileIOProcessor.hpp"
#include "async/status.hpp"

namespace HTTP
{
class Server::RequestHandler
{
  protected:
	const Request _request;
	const Server::Location &_location;
	Server *_server;
	Response _response;
	int _status;
	std::string _resource_path;
	CGI::RequestHandler *_cgi_handler;
	async::Logger &_logger;

	void registerErrorResponse(const int code, const std::exception &e);

  public:
	enum response_status_e
	{
		RESPONSE_STATUS_OK = 0,
		RESPONSE_STATUS_AGAIN
	};

	RequestHandler(Server *server, const Request &request,
				   const Server::Location &location);
	virtual ~RequestHandler();

	int task(void);
	Response retrieve(void);
	void setCGIRequestValues(CGI::Request &cgi_request);
	void handleCGI(void);
	virtual void handleRequest(void) = 0;
	void CGIResponseToHTTPResponse(const CGI::Response &cgi_response);

	bool isDirectory(void) const;
	bool isInvalidDirectoryFormat(void) const;
};

class Server::RequestGetHandler : public Server::RequestHandler
{
  private:
	async::FileReader *_reader;

  public:
	RequestGetHandler(Server *server, const Request &request,
					  const Server::Location &location);
	virtual ~RequestGetHandler();

	virtual void handleRequest(void);
};

class Server::RequestHeadHandler : public Server::RequestHandler
{
  private:
	async::FileReader *_reader;

  public:
	RequestHeadHandler(Server *server, const Request &request,
					   const Server::Location &location);
	virtual ~RequestHeadHandler();

	virtual void handleRequest(void);
};

class Server::RequestPostHandler : public Server::RequestHandler
{
  private:
	async::FileWriter *_writer;

  public:
	RequestPostHandler(Server *server, const Request &request,
					   const Server::Location &location);
	virtual ~RequestPostHandler();

	virtual void handleRequest(void);
};

class Server::RequestPutHandler : public Server::RequestHandler
{
  private:
	async::FileWriter *_writer;

  public:
	RequestPutHandler(Server *server, const Request &request,
					  const Server::Location &location);
	virtual ~RequestPutHandler();

	virtual void handleRequest(void);
};

class Server::RequestDeleteHandler : public Server::RequestHandler
{
  public:
	RequestDeleteHandler(Server *server, const Request &request,
						 const Server::Location &location);
	virtual ~RequestDeleteHandler();

	virtual void handleRequest(void);
};
} // namespace HTTP

#endif
