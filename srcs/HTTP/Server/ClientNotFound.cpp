#include "HTTP/Server.hpp"
#include "utils/string.hpp"

using namespace HTTP;

Server::ClientNotFound::ClientNotFound(const int client_fd)
	: std::runtime_error("Client fd " + toStr(client_fd)
						 + " is not yet connected.")
{
}
