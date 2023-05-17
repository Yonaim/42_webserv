#include "HTTPRequest.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

std::string loadFileFromPath(const std::string &path)
{
	std::ifstream infile;
	std::stringstream buffer;

	infile.open(path, std::ios::in);
	if (!infile.good())
		throw(std::runtime_error("Error while opening file from " + path));
	buffer << infile.rdbuf();
	infile.close();
	return (buffer.str());
}

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cout << "Usage: " << argv[0] << " /path/to/http/request/file"
				  << std::endl;
		return (2);
	}
	std::string filecontent = loadFileFromPath(argv[1]);
	HTTP::Request req;
	int rc = req.parse(filecontent);
	std::cout << "got code " << rc << std::endl;
	return (0);
}
