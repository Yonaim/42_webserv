/**
**  이름 입력하면 랜덤 포춘 쿠키 생성하는 cgi program
**/
/* 1. 클라이언트가 index.html에 본인의 이름을 입력하고 submit 버튼을 누른다.*/
/* 2. 서버는 클라이언트로부터 GET REQUEST를 받는다.(이름은 url의 query로 전달)*/
/* 3. 서버는 request body로 들어온 이름을 서버에 저장한다. */
/* 4. 서버는 저장된 이름으로 cgi 프로그램을 실행한다.	   */
/* 5. 환영 문구와 함께 포춘 쿠키 완성~ */
/* 6. cgi 프로그램 결과(html 페이지)를 클라이언트에게 보낸다. */
/* 7. 우와 포춘 쿠키다~ 									*/

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <map>

void printError(void)
{
	std::cout << "Content-Type: text/html\r\n";
	std::cout << "Status: 600 Invalid data\r\n\r\n";
	std::cout << "<html>\n";
	std::cout << "<head>\n";
	std::cout << "<title>Invalid data</title>\n";
	std::cout << "</head>\n";
	std::cout << "<h1>Invalid data typed</h1>\n";
	std::cout << "</html>\n";
}

void printResult(const std::string &name, const std::string &result)
{
	std::cout << "Content-Type: text/html\r\n";
	std::cout << "Status: 200 ok\r\n\r\n";
	std::cout << "<html>\n";
	std::cout << "<head>\n";
	std::cout << "<title>Fortune Cookie</title>\n";
	std::cout << "<style>\n";
	std::cout << "body {\n";
	std::cout << "  background-color: #f5f5f5;\n";
	std::cout << "  font-family: Arial, sans-serif;\n";
	std::cout << "  text-align: center;\n";
	std::cout << "}\n";
	std::cout << "h1 {\n";
	std::cout << "  color: #333333;\n";
	std::cout << "}\n";
	std::cout << "p {\n";
	std::cout << "  color: #666666;\n";
	std::cout << "  font-size: 18px;\n";
	std::cout << "}\n";
	std::cout << "</style>\n";
	std::cout << "</head>\n";
	std::cout << "<body>\n";
	if (name.length() > 0)
		std::cout << "<h1>Welcome, " << name << "!</h1>\n";
	else
		std::cout << "<h1>Welcome!</h1>\n";
	std::cout << "<img src=\"../fortune/img/fortune_cookie_image.jpg\" alt=\"Fortune Cookie "
				 "Image\">\n";
	std::cout << "<h1>Your Fortune : " << result << "</h1>\n";
	std::cout << "</body>\n";
	std::cout << "</html>\n";
}

std::string getRandomFortune(void)
{
	static const std::string fortunes[]
		= {"Nothing astonishes men so much as common sense and plain dealing.",
		   "The greatest risk is not taking one.",
		   "You are very talented in many ways.",
		   "The man or woman you desire feels the same about you.",
		   "You already know the answer to the questions lingering inside your "
		   "head.",
		   "You learn from your mistakes... You will learn a lot today.",
		   "Never give up. You're not a failure if you don't give up.",
		   "Be on the lookout for coming events; They cast their shadows "
		   "beforehand."};
	const int n_fortunes = sizeof(fortunes) / sizeof(fortunes[0]);

	std::srand(std::time(NULL));
	int idx = std::rand() % n_fortunes;

	return (fortunes[idx]);
}

std::string getQueryParam(const std::string &query, const std::string &param)
{
	size_t param_start_idx = query.find(param + "=");

	if (param_start_idx != std::string::npos)
	{
		param_start_idx += param.length() + 1;
		size_t param_end_idx = query.find("&", param_start_idx);
		if (param_end_idx == std::string::npos)
			param_end_idx = query.length();
		return (query.substr(param_start_idx, param_end_idx - param_start_idx));
	}
	return ("");
}

std::string getEnv(const char *var)
{
	char *ptr = std::getenv(var);

	if (ptr == NULL)
		return ("");
	else
		return (ptr);
}

int main(void)
{
	std::string query = getEnv("QUERY_STRING");
	std::string method = getEnv("REQUEST_METHOD");

	if (method == "GET")
	{
		const std::string param_name = "name";
		const std::string name = getQueryParam(query, param_name);
		const std::string fortune = getRandomFortune();
		printResult(name, fortune);
	}
	else
	{
		printError();
	};
	return (0);
}
