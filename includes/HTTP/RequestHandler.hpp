#ifndef REQUESTHANDLER_HPP
#define REQUESTHANDLER_HPP

#include "HTTP/Request.hpp"
#include "HTTP/Response.hpp"
#include "HTTP/Server.hpp"
#include "async/FileIOProcessor.hpp"
#include "async/JobStatus.hpp"

namespace HTTP
{
class Server::RequestHandler
{
  private:
	RequestHandler(void);
	RequestHandler(const RequestHandler &orig);
	RequestHandler &operator=(const RequestHandler &orig);

  protected:
	const Request _request;
	Response _response;
	int _status;
	Server const *_server;
	std::string _resource_path;

  public:
	enum response_status_e
	{
		RESPONSE_STATUS_OK = 0,
		RESPONSE_STATUS_AGAIN
	};

	RequestHandler(Server const *server, const Request &request);
	virtual ~RequestHandler();

	virtual int task(void) = 0;
	Response retrieve(void);
};

class Server::RequestGetHandler : public Server::RequestHandler
{
  private:
	async::FileReader _reader;
	RequestGetHandler(const RequestGetHandler &orig);
	RequestGetHandler &operator=(const RequestGetHandler &orig);

  public:
	RequestGetHandler(Server const *server, const Request &request);
	virtual ~RequestGetHandler();

	virtual int task(void);
};

class Server::RequestHeadHandler : public Server::RequestHandler
{
  private:
	async::FileReader _reader;
	RequestHeadHandler(const RequestHeadHandler &orig);
	RequestHeadHandler &operator=(const RequestHeadHandler &orig);

  public:
	RequestHeadHandler(Server const *server, const Request &request);
	virtual ~RequestHeadHandler();

	virtual int task(void);
};

class Server::RequestPostHandler : public Server::RequestHandler
{
  private:
	async::FileWriter _writer;
	RequestPostHandler(const RequestPostHandler &orig);
	RequestPostHandler &operator=(const RequestPostHandler &orig);

  public:
	RequestPostHandler(Server const *server, const Request &request);
	virtual ~RequestPostHandler();

	virtual int task(void);
};

class Server::RequestDeleteHandler : public Server::RequestHandler
{
  private:
	RequestDeleteHandler(const RequestDeleteHandler &orig);
	RequestDeleteHandler &operator=(const RequestDeleteHandler &orig);

  public:
	RequestDeleteHandler(Server const *server, const Request &request);
	virtual ~RequestDeleteHandler();

	virtual int task(void);
};
} // namespace HTTP

#endif
