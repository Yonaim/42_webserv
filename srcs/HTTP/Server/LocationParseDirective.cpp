#include "../const_values.hpp"
#include "ConfigDirective.hpp"
#include "HTTP/Server.hpp"
#include "utils/string.hpp"

using namespace HTTP;

void Server::Location::parseDirectiveRoot(const ConfigContext &location_context)
{
	const char *dir_name = "root";
	if (location_context.countDirectivesByName(dir_name) != 1)
	{
		_logger << location_context.name() << " should have 1 " << dir_name
				<< async::error;
		location_context.throwException(PARSINGEXC_INVALID_N_DIR);
	}
	const ConfigDirective &root_directive
		= location_context.getNthDirectiveByName(dir_name, 0);
	if (root_directive.is_context())
	{
		_logger << dir_name << " should not be context" << async::error;
		root_directive.throwException(PARSINGEXC_UNDEF_DIR);
	}
	if (root_directive.nParameters() != 1)
	{
		_logger << dir_name << " should have 1 parameter(s)" << async::error;
		root_directive.throwException(PARSINGEXC_INVALID_N_ARG);
	}
	_root = root_directive.parameter(0);
}

void Server::Location::parseDirectiveLimitExcept(
	const ConfigContext &location_context)
{
	_allowed_methods.insert(METHOD_GET);
	_allowed_methods.insert(METHOD_HEAD);

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
			_logger << dir_name << " should not be context" << async::error;
			limit_except_directive.throwException(PARSINGEXC_UNDEF_DIR);
		}

		const size_t n_methods = limit_except_directive.nParameters();
		if (n_methods == 0)
		{
			_logger << dir_name << " should have more than 0 parameter(s)"
					<< async::error;
			limit_except_directive.throwException(PARSINGEXC_INVALID_N_ARG);
		}

		for (size_t j = 0; j < n_methods; j++)
		{
			const BidiMap<std::string, int>::const_iterator it
				= METHOD.find(limit_except_directive.parameter(j));
			if (it == METHOD.end())
			{
				_logger << dir_name << " has invalid HTTP method"
						<< async::error;
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
		_logger << location_context.name() << " should have 0 or 1 " << dir_name
				<< async::error;
		return_directive.throwException(PARSINGEXC_DUP_DIR);
	}
	if (return_directive.nParameters() != 2)
	{
		_logger << dir_name << " should have 2 parameter(s)" << async::error;
		return_directive.throwException(PARSINGEXC_INVALID_N_ARG);
	}
	if (!isUnsignedIntStr(return_directive.parameter(0)))
	{
		_logger << dir_name << " should have integer form first parameter"
				<< async::error;
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
		_logger << location_context.name() << " should have 0 or 1 " << dir_name
				<< async::error;
		auto_index_directive.throwException(PARSINGEXC_DUP_DIR);
	}
	if (auto_index_directive.nParameters() != 1)
	{
		_logger << dir_name << " should have 1 parameter(s)" << async::error;
		auto_index_directive.throwException(PARSINGEXC_INVALID_N_ARG);
	}
	if (auto_index_directive.parameter(0) == "off")
	{
		_logger << "autoindex set to off" << async::debug;
		return;
	}
	else if (auto_index_directive.parameter(0) == "on")
	{
		_logger << "autoindex set to on" << async::debug;
		_autoindex = true;
	}
	else
	{
		_logger << dir_name << " should have parameter either \"on\" or \"off\""
				<< async::error;
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
			_logger << dir_name << " should not be context" << async::error;
			index_directive.throwException(PARSINGEXC_UNDEF_DIR);
		}
		if (index_directive.nParameters() == 0)
		{
			_logger << dir_name << " should have more than 0 parameter(s)"
					<< async::error;
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
		_logger << location_context.name() << " should have 0 or 1 " << dir_name
				<< async::error;
		location_context.throwException(PARSINGEXC_INVALID_N_DIR);
	}
	_upload_allowed = true;
	_logger << "Uploading to " << _root << " enabled" << async::verbose;
	const ConfigDirective &upload_directive
		= location_context.getNthDirectiveByName(dir_name, 0);
	if (upload_directive.is_context())
	{
		_logger << dir_name << " should not be context" << async::error;
		upload_directive.throwException(PARSINGEXC_UNDEF_DIR);
	}
	if (upload_directive.nParameters() != 1)
	{
		_logger << dir_name << " should have 1 parameter(s)" << async::error;
		upload_directive.throwException(PARSINGEXC_INVALID_N_ARG);
	}
	_upload_store_path = upload_directive.parameter(0);
}

void Server::Location::parseDirectiveCGI(const ConfigContext &location_context)
{
	const char *dir_name = "cgi_extension";
	const size_t n_indexs = location_context.countDirectivesByName(dir_name);
	if (n_indexs == 0)
		return;
	if (n_indexs > 1)
	{
		_logger << location_context.name() << " should have 0 or 1 " << dir_name
				<< async::error;
		location_context.throwException(PARSINGEXC_INVALID_N_DIR);
	}
	_cgi_enabled = true;
	const ConfigDirective &cgi_directive
		= location_context.getNthDirectiveByName(dir_name, 0);
	if (cgi_directive.is_context())
	{
		_logger << dir_name << " should not be context" << async::error;
		cgi_directive.throwException(PARSINGEXC_UNDEF_DIR);
	}
	// 맨대토리 버젼, 1개의 아규먼트만 허용
	if (cgi_directive.nParameters() != 1)
	{
		_logger << dir_name << " should have 1 parameter(s)" << async::error;
		cgi_directive.throwException(PARSINGEXC_INVALID_N_ARG);
	}
	_cgi_extensions.insert(cgi_directive.parameter(0));
	_logger << "CGI call to " << _root << " in " << cgi_directive.parameter(0)
			<< " enabled" << async::verbose;
	/*
	보너스 대비한 버젼
	if (cgi_directive.nParameters() < 1)
	{
		_logger << dir_name << " should have more than 0 parameter(s)"
				<< async::error;
		cgi_directive.throwException(PARSINGEXC_INVALID_N_ARG);
	}
	for (size_t i = 0; i < cgi_directive.nParameters(); i++)
	{
		_cgi_extensions.insert(cgi_directive.parameter(i));
		_logger << "CGI call to " << _root << " in "
				<< cgi_directive.parameter(i) << " enabled" << async::verbose;
	}
	*/
}
