#ifndef PARSECONFIGINTERNAL_HPP
#define PARSECONFIGINTERNAL_HPP

#include "parseConfig.hpp"
#include <stdexcept>

extern const std::string token_brace_open;
extern const std::string token_brace_close;
extern const std::string token_semicolon;
extern const std::string special_tokens[3];
extern const std::string whitespace;

ConfigDirectivePtr parseConfigDirective(
	std::vector<std::string>::iterator &cursor,
	const std::vector<std::string> &tokens);
ConfigDirectivePtr parseConfigContext(
	std::vector<std::string>::iterator &cursor,
	const std::vector<std::string> &tokens);
ConfigDirectivePtr parseAvailableConfigDirective(
	std::vector<std::string>::iterator &cursor,
	const std::vector<std::string> &tokens);

#endif
