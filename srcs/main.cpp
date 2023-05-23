#include "ConfigDirective.hpp"
#include "WebServer.hpp"
#include "async/IOTaskHandler.hpp"
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
		root_context.throwException(PARSINGEXC_INVALID_N_DIR);

	const ConfigDirective &loglevel_directive
		= root_context.getNthDirectiveByName(dir_name, 0);

	if (loglevel_directive.is_context())
		root_context.throwException(PARSINGEXC_UNDEF_DIR);
	if (loglevel_directive.nParameters() != 1)
		loglevel_directive.throwException(PARSINGEXC_INVALID_N_ARG);

	async::Logger::registerFd(STDOUT_FILENO);
	async::Logger::setLogLevel(loglevel_directive.parameter(0));
	async::Logger::getLogger("root")
		<< "Set log level to " << loglevel_directive.parameter(0)
		<< async::info;
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
		async::IOTaskHandler::blockingWrite();
		std::cerr << "Error while parsing config file: " << e.what() << "\n";
		return (2);
	}

	try
	{
		parseLogLevel(rootConfig);
	}
	catch (const std::exception &e)
	{
		async::IOTaskHandler::blockingWrite();
		std::cerr << "Error while parsing log level: " << e.what() << "\n";
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
		async::IOTaskHandler::blockingWrite();
		std::cerr << "Error while running WebServer: " << e.what() << '\n';
	}

	return (0);
}
