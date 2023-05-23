#ifndef HTTP_SERVEREXCEPTIONS_HPP
#define HTTP_SERVEREXCEPTIONS_HPP

#include "HTTP/Server.hpp"

namespace HTTP
{
class Server::LocationNotFound : public std::runtime_error
{
  public:
	LocationNotFound(const std::string &path);
};
} // namespace HTTP

#endif
