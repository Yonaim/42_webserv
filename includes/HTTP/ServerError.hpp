#ifndef HTTP_SERVEREXCEPTIONS_HPP
#define HTTP_SERVEREXCEPTIONS_HPP

#include "HTTP/Server.hpp"

namespace HTTP
{
class Server::ServerError : public std::runtime_error
{
  public:
	ServerError(const std::string &why);
};
class Server::LocationNotFound : public Server::ServerError
{
  public:
	LocationNotFound(const std::string &path);
};
class Server::ClientNotFound : public Server::ServerError
{
  public:
	ClientNotFound(const int client_fd);
};
class Server::InvalidRequest : public Server::ServerError
{
  public:
	InvalidRequest(const std::string &why);
};
} // namespace HTTP

#endif
