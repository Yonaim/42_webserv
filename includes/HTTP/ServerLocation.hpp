#ifndef HTTP_SERVERLOCATION_HPP
#define HTTP_SERVERLOCATION_HPP

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
	std::string _path;
	std::string _alias;
	std::vector<std::string> _index;
	std::pair<int, std::string> _redirection;
	std::set<int> _allowed_methods;
	std::string _upload_store_path;
	size_t _max_body_size;
	async::Logger &_logger;

	void parseDirectiveAlias(const ConfigContext &location_context);
	void parseDirectiveLimitExcept(const ConfigContext &location_context);
	void parseDirectiveReturn(const ConfigContext &location_context);
	void parseDirectiveAutoIndex(const ConfigContext &location_context);
	void parseDirectiveIndex(const ConfigContext &location_context);
	void parseDirectiveUpload(const ConfigContext &location_context);
	void parseDirectiveMaxBodySize(const ConfigContext &location_context);

  public:
	Location();
	Location(const ConfigContext &location_context, const size_t max_body_size);
	~Location();
	Location(const Location &orig);
	Location &operator=(const Location &orig);

	const std::string &getPath(void) const;
	const std::string &getAlias(void) const;
	const std::string &getNthIndex(size_t nth) const;
	bool isAllowedMethod(int method) const;
	bool hasIndex() const;
	bool hasAutoIndex(void) const;
	bool doRedirect() const;
	bool uploadAllowed() const;
	Response generateRedirectResponse(void) const;
};
} // namespace HTTP
#endif
