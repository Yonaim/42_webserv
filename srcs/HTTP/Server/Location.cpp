#include "../const_values.hpp"
#include "ConfigDirective.hpp"
#include "HTTP/Server.hpp"
#include "utils/string.hpp"
#include <cctype>
#include <iostream>
#include <sstream>

void HTTP::Server::Location::parseDirectiveRoot(
	const ConfigContext &location_context)
{
	if (location_context.countDirectivesByName("root") != 1)
		location_context.throwException(PARSINGEXC_INVALID_N_DIR);
	const ConfigDirective &root_directive
		= location_context.getNthDirectiveByName("root", 0);
	if (root_directive.is_context())
		root_directive.throwException(PARSINGEXC_UNDEF_DIR);
	if (root_directive.nParameters() != 1)
		root_directive.throwException(PARSINGEXC_INVALID_N_ARG);
	_root = root_directive.parameter(0);
}

void HTTP::Server::Location::parseDirectiveLimitExcept(
	const ConfigContext &location_context)
{
	_allowed_methods.insert(METHOD_GET);
	_allowed_methods.insert(METHOD_HEAD);

	const size_t n_limit_excepts
		= location_context.countDirectivesByName("limit_except");
	if (n_limit_excepts == 0)
		return;
	_allowed_methods.clear();

	for (size_t i = 0; i < n_limit_excepts; i++)
	{
		const ConfigDirective &limit_except_directive
			= location_context.getNthDirectiveByName("limit_except", i);
		if (limit_except_directive.is_context())
			limit_except_directive.throwException(PARSINGEXC_UNDEF_DIR);

		const size_t n_methods = limit_except_directive.nParameters();
		if (n_methods == 0)
			limit_except_directive.throwException(PARSINGEXC_INVALID_N_ARG);

		for (size_t j = 0; j < n_methods; j++)
		{
			const std::map<std::string, int>::const_iterator it
				= METHOD.find(limit_except_directive.parameter(j));
			if (it == METHOD.end())
				limit_except_directive.throwException(PARSINGEXC_UNDEF_ARG);

			const int method = it->second;
			if (method == METHOD_GET)
			{
				_allowed_methods.insert(METHOD_GET);
				_allowed_methods.insert(METHOD_HEAD);
			}
			else
			{
				_allowed_methods.insert(method);
			}
		}
	}
}

void HTTP::Server::Location::parseDirectiveReturn(
	const ConfigContext &location_context)
{
	_do_redirection = false;
	const size_t n_returns = location_context.countDirectivesByName("return");
	if (n_returns == 0)
		return;
	const ConfigDirective &return_directive
		= location_context.getNthDirectiveByName("return", 0);
	if (n_returns > 1)
		return_directive.throwException(PARSINGEXC_DUP_DIR);
	if (return_directive.nParameters() != 2)
		return_directive.throwException(PARSINGEXC_INVALID_N_ARG);
	if (!isUnsignedIntStr(return_directive.parameter(0)))
		return_directive.throwException(PARSINGEXC_UNDEF_ARG);
	_do_redirection = true;
	std::stringstream buf(return_directive.parameter(0));
	buf >> _redirection.first;
	if (!(isValidStatusCode(_redirection.first)))
		return_directive.throwException(PARSINGEXC_DUP_ARG);
	_redirection.second = return_directive.parameter(1);
}

void HTTP::Server::Location::parseDirectiveAutoIndex(
	const ConfigContext &location_context)
{
	_autoindex = false;
	const size_t n_auto_indexs
		= location_context.countDirectivesByName("auto_index");
	if (n_auto_indexs == 0)
		return;
	const ConfigDirective &auto_index_directive
		= location_context.getNthDirectiveByName("auto_index", 0);
	if (n_auto_indexs > 1)
		auto_index_directive.throwException(PARSINGEXC_DUP_DIR);
	if (auto_index_directive.nParameters() != 1)
		auto_index_directive.throwException(PARSINGEXC_INVALID_N_ARG);
	if (auto_index_directive.parameter(0) == "off")
		return;
	else if (auto_index_directive.parameter(0) == "on")
		_autoindex = true;
	else
		auto_index_directive.throwException(PARSINGEXC_UNDEF_ARG);
}

void HTTP::Server::Location::parseDirectiveIndex(
	const ConfigContext &location_context)
{
	const size_t n_indexs = location_context.countDirectivesByName("index");
	if (n_indexs == 0)
		return;
	_has_index = true;
	for (size_t i = 0; i < n_indexs; i++)
	{
		const ConfigDirective &index_directive
			= location_context.getNthDirectiveByName("index", i);
		if (index_directive.is_context())
			index_directive.throwException(PARSINGEXC_UNDEF_DIR);
		if (index_directive.nParameters() == 0)
			index_directive.throwException(PARSINGEXC_INVALID_N_ARG);
		for (size_t j = 0; j < index_directive.nParameters(); j++)
			_index.push_back(index_directive.parameter(j));
	}
}

HTTP::Server::Location::Location()
{
}

HTTP::Server::Location::Location(const ConfigContext &location_context)
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

HTTP::Server::Location::~Location()
{
}

HTTP::Server::Location::Location(const Location &orig)
	: _has_index(orig._has_index), _do_redirection(orig._do_redirection),
	  _autoindex(orig._autoindex), _path(orig._path), _root(orig._root),
	  _index(orig._index), _redirection(orig._redirection),
	  _allowed_methods(orig._allowed_methods)
{
}

HTTP::Server::Location &HTTP::Server::Location::operator=(const Location &orig)
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

const std::string &HTTP::Server::Location::getPath(void) const
{
	return (_path);
}
