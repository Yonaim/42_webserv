#ifndef HTTP_REQUESTHANDLER_HPP
#define HTTP_REQUESTHANDLER_HPP

#include "CGI/RequestHandler.hpp"
#include "HTTP/Request.hpp"
#include "HTTP/Response.hpp"
#include "HTTP/Server.hpp"
#include "async/FileIOHandler.hpp"
#include "async/status.hpp"

namespace HTTP
{
class Server::RequestHandler
{
  protected:
	const Request _request;
	Response _response;
	const Server::Location &_location;
	Server *_server;
	int _status;
	int _error_code;
	std::string _resource_path;
	async::Logger &_logger;

	void setErrorCode(const int code);

  public:
	enum response_status_e
	{
		RESPONSE_STATUS_OK = 0,
		RESPONSE_STATUS_AGAIN,
		RESPONSE_STATUS_ERROR
	};

	RequestHandler(Server *server, const Request &request,
				   const Server::Location &location,
				   const std::string &resource_path);
	virtual ~RequestHandler();

	virtual int task(void) = 0;
	Response retrieve(void);
	const int &errorCode(void) const;

	bool isDirectory(void) const;
	bool isInvalidDirectoryFormat(void) const;
	const Request &getRequest(void) const;
};

class Server::RequestGetHandler : public Server::RequestHandler
{
  private:
	async::FileReader _reader;

  public:
	RequestGetHandler(Server *server, const Request &request,
					  const Server::Location &location,
					  const std::string &resource_path);
	virtual ~RequestGetHandler();

	virtual int task(void);
};

class Server::RequestHeadHandler : public Server::RequestHandler
{
  private:
	async::FileReader _reader;

  public:
	RequestHeadHandler(Server *server, const Request &request,
					   const Server::Location &location,
					   const std::string &resource_path);
	virtual ~RequestHeadHandler();

	virtual int task(void);
};

class Server::RequestPostHandler : public Server::RequestHandler
{
  private:
	async::FileWriter _writer;

  public:
	RequestPostHandler(Server *server, const Request &request,
					   const Server::Location &location,
					   const std::string &resource_path);
	virtual ~RequestPostHandler();

	virtual int task(void);
};

class Server::RequestPutHandler : public Server::RequestHandler
{
  private:
	async::FileWriter _writer;

  public:
	RequestPutHandler(Server *server, const Request &request,
					  const Server::Location &location,
					  const std::string &resource_path);
	virtual ~RequestPutHandler();

	virtual int task(void);
};

class Server::RequestDeleteHandler : public Server::RequestHandler
{
  public:
	RequestDeleteHandler(Server *server, const Request &request,
						 const Server::Location &location,
						 const std::string &resource_path);
	virtual ~RequestDeleteHandler();

	virtual int task(void);
};
} // namespace HTTP

#endif
