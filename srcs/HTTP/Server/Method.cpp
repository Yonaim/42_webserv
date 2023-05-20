#include "../const_values.hpp"
#include "HTTP/Server.hpp"
#include "HTTPServerException.hpp"
#include "async/FileIOProcessor.hpp"
#include "utils.hpp"
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unistd.h>

// read
// parse
// process
// response
//

void HTTP::Server::setErrorPage(HTTP::Response &response, int status_code)
{
	std::map<int, std::string>::const_iterator iter
		= _error_pages.find(status_code);
	std::string error_page_path;

	if (iter == _error_pages.end())
		error_page_path = _error_pages.at(0);
	else
		error_page_path = iter->second;
	// TODO: error_page_path로 error_page 파일 열기
	int fd = open(error_page_path.c_str(), O_RDONLY);
	// TODO: error_page를 여는 것을 실패했을때 어떻게 처리할지 결정할 것
	if (fd < 0)
		;
	async::FileReader reader(fd);

	reader.task();
}

void HTTP::Server::methodHandler(HTTP::Request &request, int fd)
{
	Response response;

	try
	{
		const int method = request.getMethod();
		const std::string uri_path = request.getURIPath();
		const Location location = getLocation(uri_path);
		const std::string resource_path = location.getRoot() + uri_path;

		// TODO:
		// server 설정에서 주어진 method가 허용 method에 속하는지 체크하는 부분
		switch (method)
		{
		case METHOD_GET:
			getMethodHandler(request, response);
			break;
		case METHOD_HEAD:
			headMethodHandler(request, response);
			break;
		case METHOD_POST:
			postMethodHandler(request, response);
			break;
		case METHOD_DELETE:
			deleteMethodHandler(request, response);
			break;
		default:
			throw(ServerException(501));
			// TODO:
			// 501 not implemented 에러 반환
			// 이전에 파싱한 부분 약간 수정해야할듯
			// 존재하지 않는 메소드는 METHOD_NONE으로 저장해놓고 여기서 에러
			// 핸들링하도록
		}
	}
	catch (HTTP::ServerException &e)
	{
		std::cout << e.what() << std::endl;
		setErrorPage(response, e.getStatusCode());
	}
	_output_queue[fd].push(response);
}

void HTTP::Server::setErrorPage(HTTP::Response &response, int status_code)
{
	const std::map<int, std::string>::const_iterator iter
		= _error_pages.find(status_code);
	std::string error_page;

	if (iter == _error_pages.end())
		error_page = _error_pages.at(0);
	else
		error_page = iter->second;
	// TODO:
	// 파일을 읽고, 내용을 string에 저장하면
	std::string content;
	response.setBody(content);
	response.setStatus(status_code);
}

// uri = <스킴>://<사용자
// 이름>:<비밀번호>@<호스트>:<포트>/<경로>;<파라미터>?<질의>#<프래그먼트>

void HTTP::Server::getMethodHandler(HTTP::Request &request,
									HTTP::Response &response)
{
	// TODO:
	const std::string resource_path;
	const std::ifstream resource(resource_path, std::ios::binary);
	std::stringstream buffer;

	// 해당 파일 전체를 아직 다 안읽은 상황
	// again을 호출하든지...
	// 다 읽었으면 뒤에를 실행

	if (!resource.is_open())
	{
		// 404 에러 페이지는 디폴트로 설정되어있다는 가정하에
		response.setStatus(404);
		response.setBody(_error_pages.find(404)->second);
		return;
	}
	buffer << resource.rdbuf();
	std::string content = buffer.str();

	// 성공적으로 읽었음을 설정
	response.setStatus(200);
	response.setBody(content);
	response.setContentLength(content.length());
	response.setContentType(resource_path);
}

void HTTP::Server::headMethodHandler(HTTP::Request &request,
									 HTTP::Response &response)
{
	// TODO:
	// resource path 구하는 로직 필요
	// const std::string resource_path = processURI(request.getURIPath());
	const std::string resource_path;

	const std::ifstream resource(resource_path, std::ios::binary);
	std::stringstream buffer;

	if (!resource.is_open())
	{
		setErrorPage(response, 404);
		return;
	}
	buffer << resource.rdbuf();
	std::string content = buffer.str();

	// 성공적으로 읽었음을 설정
	response.setStatus(200);
	response.setContentLength(content.length());
	response.setContentType(resource_path);
}

// POST 메소드
//		: CGI
//		: 리소스 생성
//		: 리소스에 내용 추가
void HTTP::Server::postMethodHandler(HTTP::Request &request,
									 HTTP::Response &response)
{
	// const std::string resource_path
	// 	= processURI(request.getURI()); // resource path 구하는 로직 필요

	std::ofstream resource(resource_path, std::ios::app);

	// if (/* cgi가 아니면 */)
	if (!resource.is_open())
	{
		response.setStatus(404);
		response.setBody(_error_pages.find(404)->second);
	}
	else
	{
		response.setStatus(200);
		response.setValue("Location", resource_path);
		resource << request.getBody();
	}
	// else
}

// 해당되는 URI 파일을 지운다.
void HTTP::Server::deleteMethodHandler(HTTP::Request &request,
									   HTTP::Response &response)
{
	// const std::string resource_path
	// 	= processURI(request.getURI()); // resource path 구하는 로직 필요

	if (unlink(resource_path.c_str()) == -1)
	{
		response.setStatus(404);
		response.setBody(_error_pages.find(404)->second);
	}
	else
		response.setStatus(200);
}
