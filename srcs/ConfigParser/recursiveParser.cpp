#include "ConfigDirective.hpp"
#include "parseConfigInternal.hpp"
#include <string>

bool isSpecialToken(const std::string &token)
{
	for (size_t i = 0; i < sizeof(special_tokens) / sizeof(std::string); i++)
	{
		if (special_tokens[i] == token)
			return (true);
	}
	return (false);
}

ConfigDirective parseConfigDirective(std::vector<std::string>::iterator &cursor,
									 const std::vector<std::string> &tokens)
{
	const std::string &name = *cursor;
	std::vector<std::string> parameters;
	std::vector<std::string>::iterator cursor_end = cursor;
	while (true)
	{
		cursor_end++;
		if (cursor_end == tokens.end())
			throw(ConfigDirective::InvalidDirective(
				"Reached end of tokens without semicolon."));
		if (isSpecialToken(*cursor_end))
			break;
		parameters.push_back(*cursor_end);
	}
	if (*cursor_end != token_semicolon)
		throw(ConfigDirective::InvalidDirective("Unexpected token \""
												+ *cursor_end + "\"."));
	if (cursor_end == cursor)
		throw(ConfigDirective::InvalidDirective("Not enough tokens to parse."));
	cursor = cursor_end;
	cursor++;
	return (ConfigDirective(name, parameters));
}

ConfigContext parseConfigContext(std::vector<std::string>::iterator &cursor,
								 const std::vector<std::string> &tokens)
{
	const std::string &name = *cursor;
	std::vector<std::string> parameters;
	std::vector<ConfigDirective *> directives;
	std::vector<std::string>::iterator cursor_end = cursor;
	while (true)
	{
		cursor_end++;
		if (cursor_end == tokens.end())
			throw(ConfigDirective::InvalidDirective(
				"Reached end of tokens without braces."));
		if (isSpecialToken(*cursor_end))
			break;
		parameters.push_back(*cursor_end);
	}
	if (*cursor_end != token_brace_open)
		throw(ConfigDirective::InvalidDirective("Unexpected token \""
												+ *cursor_end + "\"."));
	if (cursor_end == cursor)
		throw(ConfigDirective::InvalidDirective("Not enough tokens to parse."));
	cursor_end++;
	while (true)
	{
		if (isSpecialToken(*cursor_end) || cursor_end == tokens.end())
			break;
		directives.push_back(parseAvailableConfigDirective(cursor_end, tokens));
	}
	if (cursor_end == tokens.end())
		throw(ConfigDirective::InvalidDirective(
			"Reached end of tokens without braces."));
	if (*cursor_end != token_brace_close)
		throw(ConfigDirective::InvalidDirective("Unexpected token \""
												+ *cursor_end + "\"."));
	cursor = cursor_end;
	cursor++;
	return (ConfigContext(name, parameters, directives));
}

ConfigDirective *parseAvailableConfigDirective(
	std::vector<std::string>::iterator &cursor,
	const std::vector<std::string> &tokens)
{
	try
	{
		return (new ConfigDirective(parseConfigDirective(cursor, tokens)));
	}
	catch (const std::exception &e)
	{
		(void)e;
	}
	return (new ConfigContext(parseConfigContext(cursor, tokens)));
}
