#include "HTTP/Server.hpp"

using namespace HTTP;

Server::LocationNotFound::LocationNotFound(const std::string &path)
	: std::runtime_error("No location with path " + path + " found.")
{
}
