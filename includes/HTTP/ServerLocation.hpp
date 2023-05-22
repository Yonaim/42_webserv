#ifndef SERVERLOCATION_HPP
#define SERVERLOCATION_HPP

#include "Server.hpp"

namespace HTTP
{
class Server::Location
{
  private:
	bool _has_index;
	bool _do_redirection;
	bool _autoindex;
	std::string _path;
	std::string _root;
	std::vector<std::string> _index;
	std::pair<int, std::string> _redirection;
	std::set<int> _allowed_methods;

  public:
	Location();
	Location(const ConfigContext &location_context);
	~Location();
	Location(const Location &orig);
	Location &operator=(const Location &orig);

	void parseDirectiveRoot(const ConfigContext &location_context);
	void parseDirectiveLimitExcept(const ConfigContext &location_context);
	void parseDirectiveReturn(const ConfigContext &location_context);
	void parseDirectiveAutoIndex(const ConfigContext &location_context);
	void parseDirectiveIndex(const ConfigContext &location_context);
	const std::string &getPath(void) const;

	// 추가
	const std::string &getRoot(void) const;
	const std::string &getNthIndex(size_t nth) const;
	bool isAllowedMethod(int method);
	bool hasIndex() const;
};
} // namespace HTTP
#endif
