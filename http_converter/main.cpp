#include "HttpReq.hpp"
#include <iostream>
#include <map>
#include <string>
#include <vector>

using http_msg::HttpReq;
using http_msg::str_vec_map_t;

int main(void)
{
	while (true)
	{
		std::string str;
		std::getline(std::cin, str, '_');
		if (str == "\n" || str == "")
			break;

		static HttpReq req(str);

		try
		{
			if (req.isChunked())
				req.appendChunk(str);
			else if (req.parse() == false)
				throw(1);
			std::cout << "method: " << req.getMethod() << std::endl;
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
		}
		catch (int e)
		{
			std::cout << "error당" << std::endl;
		}
	}
	return (0);
}
