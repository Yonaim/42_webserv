#include "parseConfigInternal.hpp"
#include <string>
#include <vector>

const std::string token_brace_open = "{";
const std::string token_brace_close = "}";
const std::string token_semicolon = ";";
const std::string special_tokens[3]
	= {token_brace_open, token_brace_close, token_semicolon};
const std::string whitespace = " \t\n";

bool isSpecialToken(const size_t cursor, const std::string &content)
{
	for (size_t i = 0; i < sizeof(special_tokens) / sizeof(std::string); i++)
	{
		if (content.substr(cursor, 1) == special_tokens[i])
			return (true);
	}
	return (false);
}

void skipWhitespace(size_t &cursor, const std::string &content)
{
	while (cursor < content.size()
		   && whitespace.find(content[cursor]) != std::string::npos)
		cursor++;
}

void skipNormalChar(size_t &cursor, const std::string &content)
{
	while (cursor < content.size()
		   && whitespace.find(content[cursor]) == std::string::npos
		   && !isSpecialToken(cursor, content))
		cursor++;
}

void splitIntoTokens(std::vector<std::string> &tokens,
					 const std::string &content)
{
	size_t cursor = 0;
	while (cursor < content.size())
	{
		skipWhitespace(cursor, content);
		size_t cursor_end = cursor + 1;
		if (!isSpecialToken(cursor, content))
			skipNormalChar(cursor_end, content);
		std::string token = content.substr(cursor, cursor_end - cursor);
		if (!token.empty())
			tokens.push_back(token);
		cursor = cursor_end;
	}
}
