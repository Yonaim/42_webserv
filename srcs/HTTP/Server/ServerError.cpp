#include "HTTP/Server.hpp"
#include "utils/string.hpp"

using namespace HTTP;

Server::ServerError::ServerError(const std::string &why)
	: std::runtime_error(why)
{
}

Server::LocationNotFound::LocationNotFound(const std::string &path)
	: Server::ServerError("No location with path " + path + " found.")
{
}

Server::ClientNotFound::ClientNotFound(const int client_fd)
	: Server::ServerError("Client fd " + toStr(client_fd)
						  + " is not yet connected.")
{
}

Server::InvalidRequest::InvalidRequest(const std::string &why)
	: Server::ServerError("Request is invalid because " + why)
{
}
