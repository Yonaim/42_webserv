#include "HTTPServer.hpp"
#include "ConfigDirective.hpp"
#include <cctype>
#include <iostream>
#include <sstream>

// TODO: 직접 NGINX를 구동해서 어떤 config파일 형식에 대해 에러로 간주하고, 어떤
// 형식에 대해 통과시켜주는지 확인해볼 것

/*
1. 처음부터 끝까지 순회하며 같은 종류의 directive들을 모아둔다 (name과 개수
체크)
	- 틀린 name을 갖는 directive가 있다면 에러
	- 중복 허용되지 않는 directive가 여러 개 있으면 에러
	- 필수적인 directive가 없으면 에러

2. 종류 별로 directive를 처리
	- listen, server_name, error_page, location순
	- 이때 형식이 알맞지 않은 directive가 있다면 에러를 던진다
*/

static bool isUnsignedIntStr(const std::string &str)
{
	for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
		if (std::isdigit(*it) == false)
			return (false);
	return (true);
}

HTTPServer::HTTPServer(const ConfigContext &server_context)
{
	std::map<std::string, std::vector<ConfigDirective> > directive_map;

	if (!server_context.isConfigValid(directives_info))
		throw(std::runtime_error("Invalid directive(s): context 'server'"));

	// 1. 처음부터 끝까지 순회하며 같은 종류의 directive들을 모아둔다
	// (name과 개수 체크)
	if (server_context.countDirectivesByName("listen") != 1
		|| server_context.countDirectivesByName("server_name") == 0)
		throw(std::runtime_error(
			"Invalid number of directive(s): context 'server'"));

	// 2. 종류 별로 directive를 처리
	for (size_t i = 0; i < server_context.nDirectives(); i++)
	{
		if (server_context.directive(i).name() == "location")
			addLocationInfo(
				(const ConfigContext &)(server_context.directive(i)));
		else
			addOtherInfo(server_context.directive(i));
	}
}

HTTPServer::~HTTPServer()
{
}

HTTPServer::HTTPServer(const HTTPServer &orig)
{
	(void)orig;
	// TODO: 복사 생성자 구현
}

HTTPServer &HTTPServer::operator=(const HTTPServer &orig)
{
	(void)orig;
	// TODO: 할당 연산자 구현
	return (*this);
}

// locations vector에 새로운 location의 string 및 객체 추가
void HTTPServer::addLocationInfo(const ConfigContext &location_context)
{
	HTTPLocation new_location(location_context);
	if (locations.find(new_location.getPath()) != locations.end())
		throw(std::runtime_error("Duplicated location path: context 'server'"));
	locations[new_location.getPath()] = new_location;
}

void HTTPServer::addOtherInfo(const ConfigDirective &directive)
{
	if (directive.name() == "listen")
	{
	}
	else if (directive.name() == "server_name")
	{
	}
	else if (directive.name() == "error_page")
	{
	}
	else
		throw(
			std::runtime_error("Undefined directive found: context 'server'"));
}

HTTPServer::HTTPLocation::HTTPLocation(const ConfigContext &location_context)
{
	// 기본값
	this->has_index = false;
	this->do_redirection = false;
	this->autoindex = false;

	if (location_context.nParameters() != 1)
		throw(std::runtime_error(
			"Invalid number of argument(s): context 'location'"));
	if (!location_context.isConfigValid(directives_info))
		throw(std::runtime_error("Invalid directive(s): context 'location'"));
	// directive의 기본적인 name과 개수 확인
	if (location_context.countDirectivesByName("root") != 1
		|| location_context.countDirectivesByName("limit_except") > 1
		|| location_context.countDirectivesByName("return") > 1
		|| location_context.countDirectivesByName("autoindex") > 1)
		throw(std::runtime_error(
			"Invalid number of directive(s): context 'location'"));

	// directive 처리
	// root
	const ConfigDirective &d_root
		= location_context.getNthDirectiveByName("root", 0);
	if (d_root.nParameters() != 1)
		throw("Invalid number of argument(s): context 'root'");
	this->root = d_root.parameter(0);

	// limit_except
	if (location_context.isDirectiveExist("limit_except") == true)
	{
		const ConfigDirective d_limit_except
			= location_context.getNthDirectiveByName("limit_except", 0);
		for (size_t i = 0; i < d_limit_except.nParameters(); i++)
		{
			if (http_methods.find(d_limit_except.parameter(i))
				== http_methods.end())
				throw(std::runtime_error(
					"Undefined argument found: directive 'limit_except'"));
			this->allowed_methods.insert(
				http_methods.find(d_limit_except.parameter(i))->second);
		}
	}
	else
	{
		this->allowed_methods.insert(GET);
		this->allowed_methods.insert(HEAD);
	}

	// return
	if (location_context.isDirectiveExist("return") == true)
	{
		do_redirection = true;
		const ConfigDirective d_return
			= location_context.getNthDirectiveByName("return", 0);
		if (d_return.nParameters() != 2)
			throw("Invalid number of argument(s): directive 'return'");
		if (isUnsignedIntStr(d_return.parameter(0)) == false)
			throw(std::runtime_error(
				"Undefined argument found: directive 'return'"));

		// ss.fail() 리턴값 확인으로 대체할 수 있는지 체크해볼 것
		std::stringstream ss(d_return.parameter(0));
		ss >> this->redirection.first;
		this->redirection.second = d_return.parameter(1);
	}

	// autoindex
	if (location_context.isDirectiveExist("autoindex") == true)
	{
		const ConfigDirective &d_autoindex
			= location_context.getNthDirectiveByName("autoindex", 0);
		if (d_autoindex.nParameters() != 1)
			throw("Invalid number of argument(s): directive 'autoindex'");
		if (d_autoindex.parameter(0) == "on")
			this->autoindex = true;
		else if (d_autoindex.parameter(0) == "off")
			;
		else
			throw(std::runtime_error(
				"Undefined argument found: directive 'autoindex'"));
	}

	// index
	if (location_context.isDirectiveExist("index") == true)
	{
		for (size_t i = 0; i < location_context.countDirectivesByName("index");
			 i++)
		{
			const ConfigDirective &d_index
				= location_context.getNthDirectiveByName("index", i);
			for (size_t j = 0; j < d_index.nParameters(); j++)
			{
				this->index.push_back(d_index.parameter(j));
			}
		}
	}
}
