#include "../const_values.hpp"
#include "HTTP/ServerLocation.hpp"

const std::string &HTTP::Server::Location::getRoot(void) const
{
	return (_root);
}

bool HTTP::Server::Location::isAllowedMethod(int method) const
{
	const std::set<int>::const_iterator iter = _allowed_methods.find(method);

	return (iter != _allowed_methods.end());
}

const HTTP::Server::Location &HTTP::Server::getLocation(
	const std::string &path) const
{
	std::map<std::string, Location>::const_iterator iter = _locations.begin();

	for (; iter != _locations.end(); ++iter)
	{
		const std::string key = iter->first;

		if (path.find(key) == 0)
			break;
	}

	if (iter == _locations.end())
		throw(LocationNotFound(path));
	return (iter->second);
}

std::string HTTP::Server::getResourcePath(const Request &req) const
{
	const std::string &uri_path = req.getURIPath();
	const Location &location = getLocation(uri_path);

	if (uri_path.back() == '/' && location.hasIndex())
		return (location.getRoot() + uri_path + location.getNthIndex(0));

	return (location.getRoot() + uri_path);
}

bool HTTP::Server::Location::hasIndex() const
{
	return (_has_index);
}

const std::string &HTTP::Server::Location::getNthIndex(size_t nth) const
{
	if (nth > _index.size())
		throw(std::runtime_error("exceeded the accessible range"));
	return (_index[nth]);
}
