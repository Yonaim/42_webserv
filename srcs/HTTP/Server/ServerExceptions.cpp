#include "HTTP/Server.hpp"
#include "utils/string.hpp"

using namespace HTTP;

Server::LocationNotFound::LocationNotFound(const std::string &path)
	: std::runtime_error("No location with path " + path + " found.")
{
}

Server::ClientNotFound::ClientNotFound(const int client_fd)
	: std::runtime_error("Client fd " + toStr(client_fd)
						 + " is not yet connected.")
{
}

Server::InvalidRequest::InvalidRequest(const std::string &why)
	: std::runtime_error("Request is invalid because " + why)
{
}
