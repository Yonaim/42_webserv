#include "HTTPServer.hpp"
#include "../ConfigParser/ConfigDirective.hpp"
#include <cctype>
#include <iostream>

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

// server context를 받아 HTTPServer 객체를 반환하는 함수
HTTPServer::HTTPServer(const ConfigContext &server_context)
{
	try
	{
		std::map<std::string, std::vector<ConfigDirective>> directive_map;
		std::vector<std::pair<std::string, bool>> server_directives_info{
			{"listen", false},
			{"server_name", false},
			{"error_page", false},
			{"location", true}};

		// 1. 처음부터 끝까지 순회하며 같은 종류의 directive들을 모아둔다
		// (name과 개수 체크)
		directive_map = getDirectiveMap(server_context, server_directives_info);
		if (directive_map.find("listen")->second.size() != 1
			|| directive_map.find("server_name")->second.size() == 0)
			throw(std::runtime_error("number of specific directive line"));

		// 2. 종류 별로 directive를 처리
		for (std::map<std::string, std::vector<ConfigDirective>>::iterator it
			 = directive_map.begin();
			 it != directive_map.end(); it++)
		{
			if (it->first == "location")
				for (int i = 0; i < it->second.size(); i++)
					addLocationInfo((const ConfigContext &)(it->second.at(i)));
			else
				for (int i = 0; i < it->second.size(); i++)
					addOtherInfo((it->second.at(i)));
		}
	}
	catch (const std::exception &e)
	{
		std::cerr << "Error: Invalid Configuration file: " << '\n';
		std::cerr << e.what() << '\n';
	}
}

// context와 해당 context가 가질 수 있는 directive의 정보를 넘겨 map을 얻는 함수
std::map<std::string, std::vector<ConfigDirective>> &HTTPServer::
	getDirectiveMap(const ConfigContext &context,
					std::vector<std::pair<std::string, bool>> infos)
{
	std::map<std::string, std::vector<ConfigDirective>> map;
	for (int i = 0; i < context.nDirectives(); i++)
	{
		const ConfigDirective &cur_directive = context.directive(i);
		std::vector<std::pair<std::string, bool>>::iterator it = infos.begin();
		while (it != infos.end())
		{
			if (it->first == cur_directive.name()
				&& it->second == cur_directive.is_context())
				break;
			it++;
		}
		if (it == infos.end())
			throw(std::runtime_error("Undefined directive name or format"));
		map.find(cur_directive.name())->second.push_back(cur_directive);
	}
	return (map);
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
	// 파라미터 개수 체크
	if (location_context.nParameters() != 1)
		throw(std::runtime_error(
			"The location context does not has one parameter"));
	// path 중복 여부 체크
	if (locations.find(location_context.parameter(0)) != locations.end())
		throw(std::runtime_error("duplicated location path"));

	locations.insert(
		std::make_pair(location_context.parameter(0), location_context));
}

bool HTTPServer::isNumericString(const std::string &str)
{
	for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
		if (std::isdigit(*it) == false)
			return (false);
	return (true);
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
		throw(std::runtime_error("undefined directive name"));
}

HTTPServer::HTTPLocation::HTTPLocation(const ConfigContext &location_context)
{
	// 기본값
	has_index = false;
	do_redirection = false;
	autoindex = false;

	std::map<std::string, std::vector<ConfigDirective>> directive_map;
	std::vector<std::pair<std::string, bool>> location_directives_info{
		{"root", false},
		{"limit_except", false},
		{"return", false},
		{"autoindex", false},
		{"index", false}};

	// directive의 기본적인 name과 개수 확인
	directive_map = HTTPServer::getDirectiveMap(location_context,
												location_directives_info);
	if (directive_map.find("root")->second.size() != 1
		|| directive_map.find("limit_except")->second.size() > 1
		|| directive_map.find("return")->second.size() > 1
		|| directive_map.find("autoindex")->second.size() > 1)
		throw(std::runtime_error("number of specific directive line"));

	// directive 처리
	// root
	if (directive_map.find("root")->second.at(0).nParameters() != 1)
		throw("parameter of 'root'");
	root = directive_map.find("root")->second.at(0).parameter(0);

	// limit_except
	if (directive_map.find("limit_except")->second.size() == 0)
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
