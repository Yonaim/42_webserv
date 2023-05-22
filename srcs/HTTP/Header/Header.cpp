#include "HTTP/Header.hpp"

HTTP::Header::Header()
{
}

HTTP::Header::~Header()
{
}

HTTP::Header::Header(const Header &orig) : _values(orig._values)
{
}

HTTP::Header &HTTP::Header::operator=(const HTTP::Header &orig)
{
	_values = orig._values;
	return (*this);
}

bool HTTP::Header::hasValue(std::string const &name) const
{
	HTTP::Header::const_iterator name_iter = _values.find(name);
	if (name_iter != _values.end())
		return (true);
	return (false);
}

bool HTTP::Header::hasValue(const std::string &name,
							const std::string &value) const
{
	HTTP::Header::const_iterator name_iter = _values.find(name);
	if (name_iter == _values.end())
		return (false);

	for (_list::const_iterator value_iter = name_iter->second.begin();
		 value_iter != name_iter->second.end(); ++value_iter)
	{
		if (*value_iter == value)
			return (true);
	}
	return (false);
}

bool HTTP::Header::hasValue(const HTTP::Header::const_iterator &iter,
							const std::string &value) const
{
	for (_list::const_iterator value_iter = iter->second.begin();
		 value_iter != iter->second.end(); ++value_iter)
	{
		if (*value_iter == value)
			return (true);
	}
	return (false);
}

const std::string &HTTP::Header::getValue(const std::string &name,
										  int idx) const
{
	HTTP::Header::const_iterator name_iter = _values.find(name);
	if (name_iter == _values.end())
		throw(std::runtime_error("Header name " + name + " not found."));

	return (name_iter->second.at(idx));
}

const std::vector<std::string> HTTP::Header::getValues(
	const std::string &name) const
{
	HTTP::Header::const_iterator name_iter = _values.find(name);
	if (name_iter == _values.end())
		throw(std::runtime_error("Header name " + name + " not found."));
	return (name_iter->second);
}

void HTTP::Header::assign(const std::string &name,
						  const std::vector<std::string> values)
{
	_values[name] = values;
}

void HTTP::Header::insert(const std::string &name,
						  const std::vector<std::string> values)
{
	_values[name].insert(_values[name].end(), values.begin(), values.end());
}

void HTTP::Header::insert(const std::string &name,
						  const std::string value)
{
	_values[name].insert(_values[name].end(), value);
}


HTTP::Header::iterator HTTP::Header::begin(void)
{
	return (_values.begin());
}

HTTP::Header::iterator HTTP::Header::end(void)
{
	return (_values.end());
}

HTTP::Header::const_iterator HTTP::Header::begin(void) const
{
	return (_values.begin());
}

HTTP::Header::const_iterator HTTP::Header::end(void) const
{
	return (_values.end());
}
