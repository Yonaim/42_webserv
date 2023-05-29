#include "../const_values.hpp"
#include "ConfigDirective.hpp"
#include "HTTP/Server.hpp"
#include "utils/string.hpp"

using namespace HTTP;

Server::Location::Location() : _logger(async::Logger::getLogger("Location"))
{
}

Server::Location::Location(const ConfigContext &location_context,
						   const size_t max_body_size)
	: _has_index(false), _do_redirection(false), _autoindex(false),
	  _upload_allowed(false), _max_body_size(max_body_size),
	  _logger(async::Logger::getLogger("Location"))
{
	if (location_context.nParameters() != 1)
		location_context.throwException(PARSINGEXC_INVALID_N_ARG);
	_path = location_context.parameter(0);

	parseDirectiveAlias(location_context);
	parseDirectiveLimitExcept(location_context);
	parseDirectiveReturn(location_context);
	parseDirectiveAutoIndex(location_context);
	parseDirectiveIndex(location_context);
	parseDirectiveUpload(location_context);
	parseDirectiveMaxBodySize(location_context);
}

Server::Location::~Location()
{
}

Server::Location::Location(const Location &orig)
	: _has_index(orig._has_index), _do_redirection(orig._do_redirection),
	  _autoindex(orig._autoindex), _upload_allowed(orig._upload_allowed),
	  _path(orig._path), _alias(orig._alias), _index(orig._index),
	  _redirection(orig._redirection), _allowed_methods(orig._allowed_methods),
	  _upload_store_path(orig._upload_store_path),
	  _max_body_size(orig._max_body_size), _logger(orig._logger)
{
}

Server::Location &Server::Location::operator=(const Location &orig)
{
	_has_index = orig._has_index;
	_do_redirection = orig._do_redirection;
	_autoindex = orig._autoindex;
	_upload_allowed = orig._upload_allowed;
	_path = orig._path;
	_alias = orig._alias;
	_index = orig._index;
	_redirection = orig._redirection;
	_allowed_methods = orig._allowed_methods;
	_upload_store_path = orig._upload_store_path;
	_max_body_size = orig._max_body_size;
	return (*this);
}

const std::string &Server::Location::getPath(void) const
{
	return (_path);
}

const std::string &Server::Location::getAlias(void) const
{
	return (_alias);
}

const std::string &Server::Location::getNthIndex(size_t nth) const
{
	if (nth > _index.size())
		throw(std::runtime_error("exceeded the accessible range"));
	return (_index[nth]);
}

const std::string &Server::Location::getUploadPath(void) const
{
	return (_upload_store_path);
}

std::string Server::Location::generateResourcePath(const Request &req) const
{
	std::string uri_path = req.getURIPath();

	_logger << async::verbose << __func__ << ": URI path before replace \""
			<< uri_path;
	if ((req.getMethod() == METHOD_POST || req.getMethod() == METHOD_PUT)
		&& _upload_allowed)
	{
		uri_path.replace(0, _path.length(), _upload_store_path);
		_logger << "\" after \"" << uri_path << "\"";
		_logger << async::verbose << __func__
				<< ": above path is for uploading";
		return (uri_path);
	}

	uri_path.replace(0, _path.length(), _alias);
	_logger << "\" after \"" << uri_path << "\"";
	if (req.getURIPath().back() == '/' && _has_index)
	{
		uri_path += _index[0];
		_logger << async::verbose << __func__
				<< ": add index to URI path, result: \"" << uri_path << "\"";
	}

	return (uri_path);
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

bool Server::Location::hasAutoIndex(void) const
{
	return (_autoindex);
}

bool Server::Location::doRedirect() const
{
	return (_do_redirection);
}

bool Server::Location::uploadAllowed() const
{
	return (_upload_allowed);
}

Response Server::Location::generateRedirectResponse(void) const
{
	Response response;
	response.setStatus(_redirection.first);
	response.setLocation(_redirection.second);
	return (response);
}
