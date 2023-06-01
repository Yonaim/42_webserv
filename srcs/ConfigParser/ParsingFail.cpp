#include "ConfigDirective.hpp"

static std::string generateIdentifier(const std::string &why,
									  const ConfigDirective &directive)
{
	if (directive.is_context())
		return (why + ": context '" + directive.name() + "'");
	else
		return (why + ": directive '" + directive.name() + "'");
}

ConfigDirective::InvalidDirective::InvalidDirective(
	const std::string &why, const ConfigDirective &directive)
	: std::runtime_error(generateIdentifier(why, directive))
{
}

ConfigDirective::InvalidDirective::InvalidDirective(const std::string &why)
	: std::runtime_error(why)
{
}

ConfigDirective::UndefinedDirective::UndefinedDirective(
	const ConfigDirective &directive)
	: ConfigDirective::InvalidDirective("Undefined directive(s) found",
										directive)
{
}

ConfigDirective::UndefinedArgument::UndefinedArgument(
	const ConfigDirective &directive)
	: ConfigDirective::InvalidDirective("Undefined argument(s) found",
										directive)
{
}

ConfigDirective::InvalidNumberOfDirective::InvalidNumberOfDirective(
	const ConfigDirective &directive)
	: ConfigDirective::InvalidDirective("Invalid number of directives(s)",
										directive)
{
}

ConfigDirective::InvalidNumberOfArgument::InvalidNumberOfArgument(
	const ConfigDirective &directive)
	: ConfigDirective::InvalidDirective("Invalid number of argument(s)",
										directive)
{
}

ConfigDirective::DuplicateDirective::DuplicateDirective(
	const ConfigDirective &directive)
	: ConfigDirective::InvalidDirective("Duplicate directives", directive)
{
}

ConfigDirective::DuplicateArgument::DuplicateArgument(
	const ConfigDirective &directive)
	: ConfigDirective::InvalidDirective("Duplicate arguments", directive)
{
}
