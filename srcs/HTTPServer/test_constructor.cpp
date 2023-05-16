#include "HTTPServer.hpp"
#include "parseConfig.hpp"
#include <iostream>

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cout << "Usage: " << argv[0] << " config_file_path" << std::endl;
		return (2);
	}
	try
	{
		const ConfigContext root_context = parseConfig(argv[1]);
		std::cout << root_context << "\n";
		if (root_context.countDirectivesByName("server") < 1)
			root_context.throwException(PARSINGEXC_INVALID_N_DIR);
		const ConfigContext &server_context
			= (const ConfigContext &)(root_context.getNthDirectiveByName(
				"server", 0));
		if (server_context.nParameters() != 0)
			server_context.throwException(PARSINGEXC_INVALID_N_ARG);
		const HTTPServer server(server_context);
		std::cout << "Server parsing success" << std::endl;
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
	}
	return (0);
}
