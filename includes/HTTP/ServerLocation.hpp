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
	bool _upload_allowed;
	bool _cgi_enabled;
	std::string _path;
	std::string _root;
	std::vector<std::string> _index;
	std::pair<int, std::string> _redirection;
	std::set<int> _allowed_methods;
	std::string _upload_store_path;
	std::set<std::string> _cgi_extensions; // 보너스 대비하여 set로
	async::Logger &_logger;

	void parseDirectiveRoot(const ConfigContext &location_context);
	void parseDirectiveLimitExcept(const ConfigContext &location_context);
	void parseDirectiveReturn(const ConfigContext &location_context);
	void parseDirectiveAutoIndex(const ConfigContext &location_context);
	void parseDirectiveIndex(const ConfigContext &location_context);
	void parseDirectiveUpload(const ConfigContext &location_context);
	void parseDirectiveCGI(const ConfigContext &location_context);

  public:
	Location();
	Location(const ConfigContext &location_context);
	~Location();
	Location(const Location &orig);
	Location &operator=(const Location &orig);

	const std::string &getPath(void) const;
	const std::string &getRoot(void) const;
	const std::string &getNthIndex(size_t nth) const;
	bool isAllowedMethod(int method) const;
	bool hasIndex() const;
	bool hasAutoIndex(void) const;
	bool doRedirect() const;
	bool uploadAllowed() const;
	bool cgiEnabled() const;
	bool isCGIextension(const std::string &path) const;
	Response generateRedirectResponse(void) const;
};
} // namespace HTTP
#endif
