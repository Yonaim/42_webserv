#include "HTTPServer.hpp"
#include "../ConfigParser/ConfigDirective.hpp"
#include <iostream>

/*
	1. 디폴트값 설정
	2. _directives를 처음부터 끝까지 순회하며 파싱한다
	3. 필수 directive가 모두 존재하는지 확인
		- port
		- server_name
		- root
		- allowed_methods
*/

/*
TODO: 직접 NGINX를 구동해서 어떤 config파일 형식에 대해 에러로 간주하고,
	어떤 형식에 대해 통과시켜주는지 확인해볼 것
*/

// server context를 받아 HTTPServer 객체를 반환
HTTPServer::HTTPServer(const ConfigContext &server_context)
{
	// 디폴트값 설정

	try
	{
		// _directives를 처음부터 끝까지 순회하며 파싱한다
		for (int i = 0; i < server_context.nDirectives(); i++)
		{
			const ConfigDirective &cur_directive = server_context.directive(i);
			if (cur_directive.is_context() == true)
			{
				if (server_context.directive(i).name() == "location")
					this->addLocation((ConfigContext &)cur_directive);
				// static? dynamic?
				else
					throw(std::runtime_error("Undefined context name"));
			}
			else
				this->addOtherInfo(cur_directive);
		}

		// 필수 directive가 모두 존재하는지 확인
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
		std::cerr << "Invalid Configuration format" << '\n';
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
void HTTPServer::addLocation(const ConfigContext &location_context)
{
	if (location_context.nParameters() != 1)
		throw(std::runtime_error(
			"The location context do not has one parameter"));
	// locations 안에 들어있는 원소와 중복 확인

	// location의 root는 겹치면 안되므로 map으로 변경하는게 낫지 않을까?

	locations.push_back(std::make_pair(location_context.parameter(0),
									   HTTPLocation(location_context)));
}

// location 외 다른 정보 처리
//
void HTTPServer::addOtherInfo(const ConfigDirective &directive)
{
}
