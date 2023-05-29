#include "HTTP/const_values.hpp"
#include "ConfigDirective.hpp"
#include "HTTP/Server.hpp"
#include "utils/string.hpp"

using namespace HTTP;

void Server::Location::parseDirectiveAlias(
	const ConfigContext &location_context)
{
	const char *dir_name = "alias";
	if (location_context.countDirectivesByName(dir_name) != 1)
	{
		_logger << async::error << location_context.name() << " should have 1 "
				<< dir_name;
		location_context.throwException(PARSINGEXC_INVALID_N_DIR);
	}
	const ConfigDirective &alias_directive
		= location_context.getNthDirectiveByName(dir_name, 0);
	if (alias_directive.is_context())
	{
		_logger << async::error << dir_name << " should not be context";
		alias_directive.throwException(PARSINGEXC_UNDEF_DIR);
	}
	if (alias_directive.nParameters() != 1)
	{
		_logger << async::error << dir_name << " should have 1 parameter(s)";
		alias_directive.throwException(PARSINGEXC_INVALID_N_ARG);
	}
	_alias = alias_directive.parameter(0);
}

void Server::Location::parseDirectiveLimitExcept(
	const ConfigContext &location_context)
{
	_allowed_methods.insert(METHOD_GET);

	const char *dir_name = "limit_except";
	const size_t n_limit_excepts
		= location_context.countDirectivesByName(dir_name);
	if (n_limit_excepts == 0)
		return;
	_allowed_methods.clear();

	for (size_t i = 0; i < n_limit_excepts; i++)
	{
		const ConfigDirective &limit_except_directive
			= location_context.getNthDirectiveByName(dir_name, i);
		if (limit_except_directive.is_context())
		{
			_logger << async::error << dir_name << " should not be context";
			limit_except_directive.throwException(PARSINGEXC_UNDEF_DIR);
		}

		const size_t n_methods = limit_except_directive.nParameters();
		if (n_methods == 0)
		{
			_logger << async::error << dir_name
					<< " should have more than 0 parameter(s)";
			limit_except_directive.throwException(PARSINGEXC_INVALID_N_ARG);
		}

		for (size_t j = 0; j < n_methods; j++)
		{
			const BidiMap<std::string, int>::const_iterator it
				= METHOD.find(limit_except_directive.parameter(j));
			if (it == METHOD.end())
			{
				_logger << async::error << dir_name
						<< " has invalid HTTP method";
				limit_except_directive.throwException(PARSINGEXC_UNDEF_ARG);
			}

			const int method = it->second;
			_allowed_methods.insert(method);
		}
	}
}

void Server::Location::parseDirectiveReturn(
	const ConfigContext &location_context)
{
	const char *dir_name = "return";
	_do_redirection = false;
	const size_t n_returns = location_context.countDirectivesByName(dir_name);
	if (n_returns == 0)
		return;
	const ConfigDirective &return_directive
		= location_context.getNthDirectiveByName(dir_name, 0);
	if (n_returns > 1)
	{
		_logger << async::error << location_context.name()
				<< " should have 0 or 1 " << dir_name;
		return_directive.throwException(PARSINGEXC_DUP_DIR);
	}
	if (return_directive.nParameters() != 2)
	{
		_logger << async::error << dir_name << " should have 2 parameter(s)";
		return_directive.throwException(PARSINGEXC_INVALID_N_ARG);
	}
	if (!isUnsignedIntStr(return_directive.parameter(0)))
	{
		_logger << async::error << dir_name
				<< " should have integer form first parameter";
		return_directive.throwException(PARSINGEXC_UNDEF_ARG);
	}
	_do_redirection = true;
	std::stringstream buf(return_directive.parameter(0));
	buf >> _redirection.first;
	if (!(isValidStatusCode(_redirection.first)))
		return_directive.throwException(PARSINGEXC_DUP_ARG);
	_redirection.second = return_directive.parameter(1);
}

void Server::Location::parseDirectiveAutoIndex(
	const ConfigContext &location_context)
{
	const char *dir_name = "autoindex";
	_autoindex = false;
	const size_t n_auto_indexs
		= location_context.countDirectivesByName(dir_name);
	if (n_auto_indexs == 0)
		return;
	const ConfigDirective &auto_index_directive
		= location_context.getNthDirectiveByName(dir_name, 0);
	if (n_auto_indexs > 1)
	{
		_logger << async::error << location_context.name()
				<< " should have 0 or 1 " << dir_name;
		auto_index_directive.throwException(PARSINGEXC_DUP_DIR);
	}
	if (auto_index_directive.nParameters() != 1)
	{
		_logger << async::error << dir_name << " should have 1 parameter(s)";
		auto_index_directive.throwException(PARSINGEXC_INVALID_N_ARG);
	}
	if (auto_index_directive.parameter(0) == "off")
	{
		_logger << async::debug << "autoindex set to off";
		return;
	}
	else if (auto_index_directive.parameter(0) == "on")
	{
		_logger << async::debug << "autoindex set to on";
		_autoindex = true;
	}
	else
	{
		_logger << async::error << dir_name
				<< " should have parameter either \"on\" or \"off\"";
		auto_index_directive.throwException(PARSINGEXC_UNDEF_ARG);
	}
}

void Server::Location::parseDirectiveIndex(
	const ConfigContext &location_context)
{
	const char *dir_name = "index";
	const size_t n_indexs = location_context.countDirectivesByName(dir_name);
	if (n_indexs == 0)
		return;
	_has_index = true;
	for (size_t i = 0; i < n_indexs; i++)
	{
		const ConfigDirective &index_directive
			= location_context.getNthDirectiveByName(dir_name, i);
		if (index_directive.is_context())
		{
			_logger << async::error << dir_name << " should not be context";
			index_directive.throwException(PARSINGEXC_UNDEF_DIR);
		}
		if (index_directive.nParameters() == 0)
		{
			_logger << async::error << dir_name
					<< " should have more than 0 parameter(s)";
			index_directive.throwException(PARSINGEXC_INVALID_N_ARG);
		}
		for (size_t j = 0; j < index_directive.nParameters(); j++)
			_index.push_back(index_directive.parameter(j));
	}
}

void Server::Location::parseDirectiveUpload(
	const ConfigContext &location_context)
{
	const char *dir_name = "upload_path";
	const size_t n_indexs = location_context.countDirectivesByName(dir_name);
	if (n_indexs == 0)
		return;
	if (n_indexs > 1)
	{
		_logger << async::error << location_context.name()
				<< " should have 0 or 1 " << dir_name;
		location_context.throwException(PARSINGEXC_INVALID_N_DIR);
	}
	_upload_allowed = true;
	_logger << async::verbose << "Uploading to " << _alias << " enabled";
	const ConfigDirective &upload_directive
		= location_context.getNthDirectiveByName(dir_name, 0);
	if (upload_directive.is_context())
	{
		_logger << async::error << dir_name << " should not be context";
		upload_directive.throwException(PARSINGEXC_UNDEF_DIR);
	}
	if (upload_directive.nParameters() != 1)
	{
		_logger << async::error << dir_name << " should have 1 parameter(s)";
		upload_directive.throwException(PARSINGEXC_INVALID_N_ARG);
	}
	_upload_store_path = upload_directive.parameter(0);
}

void Server::Location::parseDirectiveMaxBodySize(
	const ConfigContext &location_context)
{
	const char *dir_name = "client_max_body_size";

	if (location_context.countDirectivesByName(dir_name) == 0)
		return;
	if (location_context.countDirectivesByName(dir_name) > 1)
	{
		_logger << async::error << location_context.name()
				<< " should have 0 or 1 " << dir_name;
		location_context.throwException(PARSINGEXC_INVALID_N_DIR);
	}

	const ConfigDirective &body_size_directive
		= location_context.getNthDirectiveByName(dir_name, 0);

	if (body_size_directive.is_context())
	{
		_logger << async::error << dir_name << " should not be context";
		location_context.throwException(PARSINGEXC_UNDEF_DIR);
	}
	if (body_size_directive.nParameters() != 1)
	{
		_logger << async::error << dir_name << " should have 1 parameter(s)";
		body_size_directive.throwException(PARSINGEXC_INVALID_N_ARG);
	}

	_max_body_size = toNum<size_t>(body_size_directive.parameter(0));
	_logger << async::verbose << "max body size for " << _path << " is "
			<< _max_body_size;
}
