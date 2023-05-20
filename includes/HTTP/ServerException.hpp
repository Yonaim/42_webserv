#ifndef SERVEREXCEPTION_HPP
#define SERVEREXCEPTION_HPP

#include "Server.hpp"
#include <stdexcept>
#include <string>

namespace HTTP
{
class Server::ServerException : public std::runtime_error
{
  private:
	int _status_code;

  public:
	ServerException(int status_code);
	int getStatusCode() const;
};
} // namespace HTTP

#endif