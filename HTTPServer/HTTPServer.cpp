#include "HTTPServer.hpp"
#include "../ConfigParser/ConfigDirective.hpp"
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
}

HTTPServer &HTTPServer::operator=(const HTTPServer &orig)
{
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
	has_index = false;
	do_redirection = false;
	autoindex = false;

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
	if (location_context.getNthDirectiveByName("root", 0).nParameters() != 1)
		throw("Invalid number of argument(s): context 'root'");
	root = location_context.getNthDirectiveByName("root", 0).parameter(0);

	// limit_except
	if (location_context.countDirectivesByName("limit_except") == 0)
	{
		allowed_methods.insert(GET);
		allowed_methods.insert(HEAD);
	}
	else
	{
		// http method 문자열과 일치 확인하여 allowed methods에 넣기
	}

	// return

	// autoindex

	// index
}
