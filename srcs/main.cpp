#include "ConfigDirective.hpp"
#include "WebServer.hpp"
#include "async/SingleIOProcessor.hpp"
#include "parseConfig.hpp"
#include <iostream>
#include <signal.h>
#include <unistd.h>

void setWebServerTerminationFlag(int arg)
{
	(void)arg;
	WebServer::setTerminationFlag();
}

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
		async::Logger::blockingWriteAll();
		std::cerr << "\nError while parsing config file: " << e.what() << "\n";
		return (2);
	}

	try
	{
		parseLogLevel(rootConfig);
	}
	catch (const std::exception &e)
	{
		async::Logger::blockingWriteAll();
		std::cerr << "\nError while parsing log level: " << e.what() << "\n";
		return (2);
	}

	async::Logger &root_logger = async::Logger::getLogger("root");
	signal(SIGINT, setWebServerTerminationFlag);

	try
	{
		WebServer webserver(rootConfig);
		while (true)
		{
			int rc = webserver.task();
			if (rc != async::status::OK_AGAIN)
				break;
		}
	}
	catch (const std::exception &e)
	{
		root_logger << async::error
					<< "Error while running WebServer: " << e.what();
	}

	root_logger << async::info << "Server terminated\n";
	async::Logger::blockingWriteAll();

	return (0);
}
