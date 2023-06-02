#include "ConfigDirective.hpp"
#include "parseConfigInternal.hpp"
#include "utils/string.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

const char comment_marker = '#';

void splitIntoTokens(std::vector<std::string> &tokens,
					 const std::string &content);

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

void splitContentIntoLines(std::vector<std::string> &lines,
						   const std::string &content)
{
	std::stringstream buffer;
	buffer.str(content);
	while (!buffer.eof())
	{
		std::string line;
		std::getline(buffer, line);
		lines.push_back(line);
	}
}

void trimComments(std::string &content)
{
	std::vector<std::string> lines;
	splitContentIntoLines(lines, content);
	content = "";
	for (std::vector<std::string>::iterator line = lines.begin();
		 line != lines.end(); line++)
	{
		size_t comment_position = line->find(comment_marker);
		if (comment_position == std::string::npos)
			content += *line;
		else
			content += getfrontstr(*line, comment_position);
		content += "\n";
	}
}

std::vector<std::string> tokenizeConfig(std::string &content)
{
	trimComments(content);
	std::vector<std::string> tokens;
	splitIntoTokens(tokens, content);
	return (tokens);
}

ConfigDirectivePtr parseConfig(const std::string &path)
{
	std::string file_content = loadFileFromPath(path);
	std::vector<std::string> tokens = tokenizeConfig(file_content);
	std::vector<std::string>::iterator cursor = tokens.begin();
	std::vector<std::string> parameters;
	std::vector<ConfigDirectivePtr> directives;
	while (cursor != tokens.end())
		directives.push_back(parseAvailableConfigDirective(cursor, tokens));
	return (
		ConfigDirectivePtr(new ConfigContext("root", parameters, directives)));
}
