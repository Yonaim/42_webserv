#include "../const_values.hpp"
#include "ConfigDirective.hpp"
#include "HTTP/Server.hpp"
#include "utils/string.hpp"

using namespace HTTP;

Server::Location::Location() : _logger(async::Logger::getLogger("Location"))
{
}

Server::Location::Location(const ConfigContext &location_context)
	: _logger(async::Logger::getLogger("Location"))
{
	// 기본값
	this->_has_index = false;
	this->_do_redirection = false;
	this->_autoindex = false;

	if (location_context.nParameters() != 1)
		location_context.throwException(PARSINGEXC_INVALID_N_ARG);
	_path = location_context.parameter(0);

	parseDirectiveRoot(location_context);
	parseDirectiveLimitExcept(location_context);
	parseDirectiveReturn(location_context);
	parseDirectiveAutoIndex(location_context);
	parseDirectiveIndex(location_context);
}

Server::Location::~Location()
{
}

Server::Location::Location(const Location &orig)
	: _has_index(orig._has_index), _do_redirection(orig._do_redirection),
	  _autoindex(orig._autoindex), _path(orig._path), _root(orig._root),
	  _index(orig._index), _redirection(orig._redirection),
	  _allowed_methods(orig._allowed_methods),
	  _logger(async::Logger::getLogger("Location"))
{
}

Server::Location &Server::Location::operator=(const Location &orig)
{
	_has_index = orig._has_index;
	_do_redirection = orig._do_redirection;
	_autoindex = orig._autoindex;
	_path = orig._path;
	_root = orig._root;
	_index = orig._index;
	_redirection = orig._redirection;
	_allowed_methods = orig._allowed_methods;
	return (*this);
}

const std::string &Server::Location::getPath(void) const
{
	return (_path);
}
