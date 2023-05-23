#include "../const_values.hpp"
#include "ConfigDirective.hpp"
#include "HTTP/Server.hpp"
#include "utils/string.hpp"

using namespace HTTP;

Server::Location::Location() : _logger(async::Logger::getLogger("Location"))
{
}

Server::Location::Location(const ConfigContext &location_context)
	: _has_index(false), _do_redirection(false), _autoindex(false),
	  _upload_allowed(false), _cgi_enabled(false),
	  _logger(async::Logger::getLogger("Location"))
{
	if (location_context.nParameters() != 1)
		location_context.throwException(PARSINGEXC_INVALID_N_ARG);
	_path = location_context.parameter(0);

	parseDirectiveRoot(location_context);
	parseDirectiveLimitExcept(location_context);
	parseDirectiveReturn(location_context);
	parseDirectiveAutoIndex(location_context);
	parseDirectiveIndex(location_context);
	parseDirectiveUpload(location_context);
	parseDirectiveCGI(location_context);
}

Server::Location::~Location()
{
}

Server::Location::Location(const Location &orig)
	: _has_index(orig._has_index), _do_redirection(orig._do_redirection),
	  _autoindex(orig._autoindex), _upload_allowed(orig._upload_allowed),
	  _cgi_enabled(orig._cgi_enabled), _path(orig._path), _root(orig._root),
	  _index(orig._index), _redirection(orig._redirection),
	  _allowed_methods(orig._allowed_methods),
	  _upload_store_path(orig._upload_store_path),
	  _cgi_extensions(orig._cgi_extensions), _logger(orig._logger)
{
}

Server::Location &Server::Location::operator=(const Location &orig)
{
	_has_index = orig._has_index;
	_do_redirection = orig._do_redirection;
	_autoindex = orig._autoindex;
	_upload_allowed = orig._upload_allowed;
	_cgi_enabled = orig._cgi_enabled;
	_path = orig._path;
	_root = orig._root;
	_index = orig._index;
	_redirection = orig._redirection;
	_allowed_methods = orig._allowed_methods;
	_upload_store_path = orig._upload_store_path;
	_cgi_extensions = orig._cgi_extensions;
	return (*this);
}

const std::string &Server::Location::getPath(void) const
{
	return (_path);
}

const std::string &Server::Location::getRoot(void) const
{
	return (_root);
}

const std::string &Server::Location::getNthIndex(size_t nth) const
{
	if (nth > _index.size())
		throw(std::runtime_error("exceeded the accessible range"));
	return (_index[nth]);
}

bool Server::Location::isAllowedMethod(int method) const
{
	const std::set<int>::const_iterator iter = _allowed_methods.find(method);

	return (iter != _allowed_methods.end());
}

bool Server::Location::hasIndex() const
{
	return (_has_index);
}

bool Server::Location::uploadAllowed() const
{
	return (_upload_allowed);
}

bool Server::Location::cgiEnabled() const
{
	return (_cgi_enabled);
}

bool Server::Location::isCGIextension(const std::string &path) const
{
	std::string ext = getExtension(path);
	if (ext == "")
		return (false);
	return (_cgi_extensions.find(ext) != _cgi_extensions.end());
}
