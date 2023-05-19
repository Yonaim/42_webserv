#include "../const_values.hpp"
#include "HTTPServer.hpp"
#include <iostream>

const std::string	&HTTP::Server::Location::getRoot(void) const
{
	return (_root);
}

bool HTTP::Server::Location::isAllowedMethod(int method)
{
	const std::set<int>::const_iterator iter = _allowed_methods.find(method);

	return (iter != _allowed_methods.end());
}

const HTTP::Server::Location &HTTP::Server::getLocation(
	const std::string &location)
{
	std::map<std::string, Location>::iterator iter = _locations.find(location);

	if (iter == _locations.end())
	{
        std::cout << __func__ << "couldn't find the Location object" << std::endl;
		throw(std::runtime_error("no such location found"));
	}
    return (iter->second);
}
