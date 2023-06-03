#ifndef HTTP_HEADER_HPP
#define HTTP_HEADER_HPP

#include <map>
#include <string>
#include <vector>

/**
 * @brief name-value의 쌍으로 이루어진 field의 집합.
 * 하나의 field에는 여러 개의 value가 있을 수 있다.
 */
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
	size_t countValue(const std::string &name) const;
	const std::vector<std::string> &getValues(const std::string &name) const;
	const std::string &getValue(const std::string &name, int idx) const;

	void assign(const std::string &name, const std::vector<std::string> &val);
	void insert(const std::string &name, const std::vector<std::string> &val);
	void insert(const std::string &name, const std::string &val);
	void append(const std::string &name, const std::vector<std::string> &val);

	iterator begin(void);
	iterator end(void);
	const_iterator begin(void) const;
	const_iterator end(void) const;
};

#endif
