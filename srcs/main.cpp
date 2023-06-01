#include "ConfigDirective.hpp"
#include "WebServer.hpp"
#include "async/SingleIOProcessor.hpp"
#include "parseConfig.hpp"
#include <iostream>
#include <unistd.h>

void parseLogLevel(const ConfigContext &root_context)
{
	const char *dir_name = "log_level";

	size_t n_directives = root_context.countDirectivesByName(dir_name);
	if (n_directives == 0)
		return;
	if (n_directives > 1)
		throw(ConfigDirective::InvalidNumberOfDirective(root_context));

	const ConfigDirective &loglevel_directive
		= root_context.getNthDirectiveByName(dir_name, 0);

	if (loglevel_directive.is_context())
		throw(ConfigDirective::UndefinedDirective(root_context));
	if (loglevel_directive.nParameters() != 1)
		throw(ConfigDirective::InvalidNumberOfArgument(loglevel_directive));

	async::Logger::registerFd(STDOUT_FILENO);
	async::Logger::setLogLevel(loglevel_directive.parameter(0));
	async::Logger::getLogger("root") << async::info << "Set log level to "
									 << loglevel_directive.parameter(0);
}

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cout << "usage: " << argv[0] << " path/to/config/file\n";
		return (2);
	}

	ConfigContext rootConfig;
	try
	{
		rootConfig = parseConfig(argv[1]);
	}
	catch (const std::exception &e)
	{
		async::IOProcessor::blockingWriteAll();
		std::cerr << "\nError while parsing config file: " << e.what() << "\n";
		return (2);
	}

	try
	{
		parseLogLevel(rootConfig);
	}
	catch (const std::exception &e)
	{
		async::IOProcessor::blockingWriteAll();
		std::cerr << "\nError while parsing log level: " << e.what() << "\n";
		return (2);
	}

	try
	{
		WebServer webserver(rootConfig);
		while (true)
			webserver.task();
	}
	catch (const std::exception &e)
	{
		async::IOProcessor::blockingWriteAll();
		std::cerr << "\nError while running WebServer: " << e.what() << '\n';
	}

	return (0);
}
