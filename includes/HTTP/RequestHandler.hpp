#ifndef REQUESTHANDLER_HPP
#define REQUESTHANDLER_HPP

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

  public:
	enum response_status_e
	{
		RESPONSE_STATUS_OK = 0,
		RESPONSE_STATUS_AGAIN
	};

	RequestHandler(Server *server, const Request &request,
				   const Server::Location &location);
	virtual ~RequestHandler();

	virtual int task(void) = 0;
	Response retrieve(void);
	bool isDirectory(void) const;
	bool isInvalidDirectoryFormat(void) const;
};

class Server::RequestGetHandler : public Server::RequestHandler
{
  private:
	async::FileReader *_reader;
	void *_cgi_handler;

  public:
	RequestGetHandler(Server *server, const Request &request,
					  const Server::Location &location);
	virtual ~RequestGetHandler();

	virtual int task(void);
};

class Server::RequestHeadHandler : public Server::RequestHandler
{
  private:
	async::FileReader _reader;

  public:
	RequestHeadHandler(Server *server, const Request &request,
					   const Server::Location &location);
	virtual ~RequestHeadHandler();

	virtual int task(void);
};

class Server::RequestPostHandler : public Server::RequestHandler
{
  private:
	async::FileWriter *_writer;
	void *_cgi_handler;

  public:
	RequestPostHandler(Server *server, const Request &request,
					   const Server::Location &location);
	virtual ~RequestPostHandler();

	virtual int task(void);
};

class Server::RequestDeleteHandler : public Server::RequestHandler
{
  public:
	RequestDeleteHandler(Server *server, const Request &request,
						 const Server::Location &location);
	virtual ~RequestDeleteHandler();

	virtual int task(void);
};
} // namespace HTTP

#endif
