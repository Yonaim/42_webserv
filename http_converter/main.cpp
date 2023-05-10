#include "HttpReq.hpp"
#include <iostream>
#include <map>
#include <string>
#include <vector>

using http_msg::HttpReq;
using http_msg::str_map_t;
using http_msg::str_vec_t;

int main(void)
{
	std::string str;
	std::getline(std::cin, str, '_');

	HttpReq req(str);

	try
	{
		if (req.parse() == false)
			throw(1);
		std::cout << "method: " << req.getMethod() << std::endl;
		std::cout << "url: " << req.getUri() << std::endl;
		std::cout << "version: " << req.getVersion() << std::endl;
		std::cout << "header: " << std::endl;
		// str_map_t const &header = req.getHeader();
		// for (auto &elem : header)
		// {
		// 	std::cout << elem.first << " | ";
		// 	for (auto &elem2 : elem.second)
		// 		std::cout << "*" << elem2 << "*";
		// 	std::cout << std::endl;
		// }
		str_map_t &header = const_cast<str_map_t &>(req.getHeader());
		for (str_map_t::iterator it = header.begin(); it != header.end(); ++it)
		{
			std::cout << it->first << " | ";
			for (str_vec_t::iterator it2 = it->second.begin();
				 it2 != it->second.end(); ++it2)
				std::cout << "*" << *it2 << "*";
			std::cout << std::endl;
		}
		std::cout << std::endl;
		std::cout << "body: " << req.getBody();
	}
	catch (int e)
	{
		std::cout << "error당" << std::endl;
	}
	return (0);
}
