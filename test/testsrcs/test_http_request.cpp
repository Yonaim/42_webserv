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

void testContinuousBuffer(std::string content)
{
	std::cout << "Test : " << __func__ << std::endl;
	try
	{
		HTTP::Request req;
		int rc = req.parse(content);
		std::cout << "got code " << rc << std::endl;
	}
	catch (const std::exception &e)
	{
		std::cerr << "Error: " << e.what() << '\n';
	}
}

void testPartialBuffer(std::string content)
{
	const int n_chunks = 10;

	std::cout << "Test : " << __func__ << std::endl;
	std::string chunks[n_chunks];
	const size_t buf_size = content.size() / n_chunks;
	for (int i = 0; i < n_chunks - 1; i++)
	{
		chunks[i] = content.substr(buf_size * (size_t)i, buf_size);
	}
	chunks[n_chunks - 1] = content.substr(
		buf_size * (n_chunks - 1), content.size() - buf_size * (n_chunks - 1));

	try
	{
		std::string growing_buffer;
		HTTP::Request req;
		for (int i = 0; i < n_chunks; i++)
		{
			growing_buffer += chunks[i];
			int rc = req.parse(growing_buffer);
			std::cout << "got code " << rc << std::endl;
		}
	}
	catch (const std::exception &e)
	{
		std::cerr << "Error: " << e.what() << '\n';
	}
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

	testContinuousBuffer(filecontent);
	testPartialBuffer(filecontent);

	return (0);
}
