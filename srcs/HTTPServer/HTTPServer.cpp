#include "HTTPServer.hpp"
#include "ConfigDirective.hpp"
#include <cctype>
#include <iostream>
#include <sstream>

// TODO: 에러 메세지 문자열 생성해주는 함수 만들기
// TODO: unordered_set(C++11)을 다른 자료구조로 변경

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

void HTTPServer::parseDirectiveListen(const ConfigContext &server_context)
{
	if (server_context.countDirectivesByName("listen") != 1)
		server_context.throwException(PARSINGEXC_INVALID_N_DIR);
	const ConfigDirective &listen_directive
		= server_context.getNthDirectiveByName("listen", 0);
	if (listen_directive.is_context())
		listen_directive.throwException(PARSINGEXC_UNDEF_DIR);
	if (listen_directive.nParameters() != 1)
		listen_directive.throwException(PARSINGEXC_INVALID_N_ARG);
	// TODO: listen port 정보 파싱
}

void HTTPServer::parseDirectiveErrorPage(const ConfigContext &server_context)
{
	const size_t n_error_pages
		= server_context.countDirectivesByName("error_page");
	if (n_error_pages == 0)
		return;
	for (size_t i = 0; i < n_error_pages; i++)
	{
		const ConfigDirective &error_page_directive
			= server_context.getNthDirectiveByName("error_page", i);
		if (error_page_directive.is_context())
			error_page_directive.throwException(PARSINGEXC_UNDEF_DIR);
		// TODO: error page 정보 파싱
	}
}

void HTTPServer::parseDirectiveServerName(const ConfigContext &server_context)
{
	const size_t n_server_names
		= server_context.countDirectivesByName("server_name");
	if (n_server_names == 0)
		return;
	for (size_t i = 0; i < n_server_names; i++)
	{
		const ConfigDirective &server_name_directive
			= server_context.getNthDirectiveByName("server_name", i);
		if (server_name_directive.is_context())
			server_name_directive.throwException(PARSINGEXC_UNDEF_DIR);
		// TODO: server_name 정보 파싱
	}
}

void HTTPServer::parseDirectiveLocation(const ConfigContext &server_context)
{
	const size_t n_locations = server_context.countDirectivesByName("location");
	if (n_locations == 0)
		return;
	for (size_t i = 0; i < n_locations; i++)
	{
		const ConfigContext &location_context
			= (const ConfigContext &)(server_context.getNthDirectiveByName(
				"location", i));
		if (!location_context.is_context())
			location_context.throwException(PARSINGEXC_UNDEF_DIR);

		HTTPLocation new_location(location_context);
		if (_locations.find(new_location.getPath()) != _locations.end())
			location_context.throwException(PARSINGEXC_DUP_DIR);
		_locations[new_location.getPath()] = new_location;
	}
}

HTTPServer::HTTPServer(const ConfigContext &server_context)
{
	parseDirectiveListen(server_context);
	parseDirectiveErrorPage(server_context);
	parseDirectiveServerName(server_context);
	parseDirectiveLocation(server_context);
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

void HTTPServer::HTTPLocation::parseDirectiveRoot(
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
	// TODO: root 정보 파싱
}

void HTTPServer::HTTPLocation::parseDirectiveLimitExcept(
	const ConfigContext &location_context)
{
	const size_t n_limit_excepts
		= location_context.countDirectivesByName("limit_except");
	if (n_limit_excepts == 0)
		return;
	for (size_t i = 0; i < n_limit_excepts; i++)
	{
		const ConfigDirective &limit_except_directive
			= location_context.getNthDirectiveByName("limit_except", i);
		if (limit_except_directive.is_context())
			limit_except_directive.throwException(PARSINGEXC_UNDEF_DIR);
		// TODO: limit_except 정보 파싱
	}
}

void HTTPServer::HTTPLocation::parseDirectiveReturn(
	const ConfigContext &location_context)
{
	const size_t n_returns = location_context.countDirectivesByName("return");
	if (n_returns == 0)
		return;
	const ConfigDirective &return_directive
		= location_context.getNthDirectiveByName("return", 0);
	if (n_returns > 1)
		return_directive.throwException(PARSINGEXC_DUP_DIR);
	// TODO: return 정보 파싱
}

void HTTPServer::HTTPLocation::parseDirectiveAutoIndex(
	const ConfigContext &location_context)
{
	const size_t n_auto_indexs
		= location_context.countDirectivesByName("auto_index");
	if (n_auto_indexs == 0)
		return;
	const ConfigDirective &auto_index_directive
		= location_context.getNthDirectiveByName("auto_index", 0);
	if (n_auto_indexs > 1)
		auto_index_directive.throwException(PARSINGEXC_DUP_DIR);
	// TODO: auto_index 정보 파싱
}

void HTTPServer::HTTPLocation::parseDirectiveIndex(
	const ConfigContext &location_context)
{
	const size_t n_indexs = location_context.countDirectivesByName("index");
	if (n_indexs == 0)
		return;
	for (size_t i = 0; i < n_indexs; i++)
	{
		const ConfigDirective &index_directive
			= location_context.getNthDirectiveByName("index", i);
		if (index_directive.is_context())
			index_directive.throwException(PARSINGEXC_UNDEF_DIR);
		// TODO: index 정보 파싱
	}
}

HTTPServer::HTTPLocation::HTTPLocation(const ConfigContext &location_context)
{
	// 기본값
	this->_has_index = false;
	this->_do_redirection = false;
	this->_autoindex = false;

	if (location_context.nParameters() != 1)
		location_context.throwException(PARSINGEXC_INVALID_N_ARG);

	parseDirectiveRoot(location_context);
	parseDirectiveLimitExcept(location_context);
	parseDirectiveReturn(location_context);
	parseDirectiveAutoIndex(location_context);
	parseDirectiveIndex(location_context);
}
