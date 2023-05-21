#include "../const_values.hpp"
#include "HTTP/ServerException.hpp"
#include "HTTP/ServerLocation.hpp"
#include <iostream>

const std::string &HTTP::Server::Location::getRoot(void) const
{
	return (_root);
}

bool HTTP::Server::Location::isAllowedMethod(int method)
{
	const std::set<int>::const_iterator iter = _allowed_methods.find(method);

	return (iter != _allowed_methods.end());
}

const HTTP::Server::Location &HTTP::Server::getLocation(
	const std::string &location) const
{
	std::map<std::string, Location>::const_iterator iter
		= _locations.find(location);

	if (iter == _locations.end())
	{
		std::cout << __func__ << "couldn't find the Location object"
				  << std::endl;
		throw(ServerException(404));
	}
	return (iter->second);
}

std::string HTTP::Server::getResourcePath(const Request &req) const
{
	const int method = req.getMethod();
	const std::string uri_path = req.getURIPath();
	const Location location = getLocation(uri_path);

	return (location.getRoot() + uri_path);
}
