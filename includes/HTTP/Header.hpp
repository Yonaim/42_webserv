#ifndef HTTP_HEADER_HPP
#define HTTP_HEADER_HPP

#include <map>
#include <string>
#include <vector>

namespace HTTP
{
class Header
{
  private:
	typedef std::vector<std::string> _list;
	typedef std::map<std::string, _list> _container;
	_container _values;

  public:
	typedef _container::iterator iterator;
	typedef _container::const_iterator const_iterator;

	Header();
	~Header();
	Header(const Header &orig);
	Header &operator=(const Header &orig);

	bool hasValue(std::string const &name) const;
	bool hasValue(const std::string &name, const std::string &value) const;
	bool hasValue(const const_iterator &iter, const std::string &value) const;
	const std::vector<std::string> getValues(const std::string &name) const;
	const std::string &getValue(const std::string &name, int idx) const;

	void assign(const std::string &name, const std::vector<std::string> values);
	void insert(const std::string &name, const std::vector<std::string> values);
	void insert(const std::string &name, const std::string value);

	iterator begin(void);
	iterator end(void);
	const_iterator begin(void) const;
	const_iterator end(void) const;
};
} // namespace HTTP

#endif
