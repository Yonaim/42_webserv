#include "HttpReq.hpp"
#include <iostream>
#include <map>
#include <string>
#include <vector>

using http_msg::HTTPReq;
using http_msg::str_vec_map_t;

int main(void)
{
	std::string	typeStr[] = {"complete", "chunk header", "chunk", "trailer"};
	std::string	methodStr[] = {"GET", "POST", "DELETE"};

	while (true)
	{
		std::string str;
		std::getline(std::cin, str, '_');
		if (str == "\r\n" || str == "\n" || str == "")
			break ;

		try
		{
			HTTPReq req(str);

			std::cout << "----------------------------------------" << std::endl;
			std::cout << "type: " << typeStr[req.getType()] << std::endl;
			std::cout << "method: " << methodStr[req.getMethod()] << std::endl;
			std::cout << "url: " << req.getUri() << std::endl;
			std::cout << "version: " << req.getVersion() << std::endl;
			std::cout << "header: " << std::endl;
			str_vec_map_t &header = const_cast<str_vec_map_t &>(req.getHeader());
			for (str_vec_map_t::iterator it = header.begin(); it != header.end();
				 ++it)
			{
				std::cout << "\t" << it->first << ":" << std::endl;
				for (std::vector<std::string>::iterator it2 = it->second.begin();
					 it2 != it->second.end(); ++it2)
					std::cout << "\t\t" << *it2 << std::endl;
			}
			std::cout << "body: " << req.getBody() << std::endl;
			std::cout << "----------------------------------------" << std::endl << std::endl;
		}
		catch (std::exception &e)
		{
			std::cout << e.what() << std::endl;
		}
	}
	return (0);
}
