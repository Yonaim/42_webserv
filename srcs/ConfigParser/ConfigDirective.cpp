#include "ConfigDirective.hpp"

ConfigDirective::ConfigDirective(const std::string name)
	: _name(name), _is_context(false)
{
}

ConfigDirective::ConfigDirective(const std::string &name,
								 const std::vector<std::string> &parameters)
	: _name(name), _parameters(parameters), _is_context(false)
{
}

ConfigDirective::~ConfigDirective()
{
}

ConfigDirective::ConfigDirective(const ConfigDirective &orig)
	: _name(orig._name), _parameters(orig._parameters),
	  _is_context(orig._is_context)
{
}

ConfigDirective &ConfigDirective::operator=(const ConfigDirective &orig)
{
	_name = orig._name;
	_parameters = orig._parameters;
	_is_context = orig._is_context;
	return (*this);
}

bool ConfigDirective::is_context(void) const
{
	return (_is_context);
}

const std::string &ConfigDirective::name(void) const
{
	return (_name);
}

const std::string &ConfigDirective::parameter(size_t index) const
{
	if (index >= _parameters.size())
		throw(std::domain_error("Index out of bound."));
	return (_parameters[index]);
}

size_t ConfigDirective::nParameters(void) const
{
	return (_parameters.size());
}

ConfigContext::ConfigContext(const std::string name) : ConfigDirective(name)
{
	_is_context = true;
}

ConfigContext::ConfigContext(const std::string &name,
							 const std::vector<std::string> &parameters,
							 const std::vector<ConfigDirective *> &directives)
	: ConfigDirective(name, parameters), _directives(directives)
{
	_is_context = true;
}

ConfigContext::~ConfigContext()
{
}

ConfigContext::ConfigContext(const ConfigContext &orig)
	: ConfigDirective(orig), _directives(orig._directives)
{
}

ConfigContext &ConfigContext::operator=(const ConfigContext &orig)
{
	ConfigDirective::operator=(orig);
	_directives = orig._directives;
	return (*this);
}

const ConfigDirective &ConfigContext::directive(size_t index) const
{
	if (index >= _directives.size())
		throw(std::domain_error("Index out of bound."));
	return (*(_directives[index]));
}

size_t ConfigContext::nDirectives(void) const
{
	return (_directives.size());
}

size_t ConfigContext::countDirectivesByName(const std::string &name) const
{
	size_t counter = 0;
	for (size_t i = 0; i < _directives.size(); i++)
	{
		if (_directives[i]->name() == name)
			counter++;
	}
	return (counter);
}

const ConfigDirective &ConfigContext::getNthDirectiveByName(
	const std::string &name, size_t n) const
{
	size_t counter = 0;
	for (size_t i = 0; i < _directives.size(); i++)
	{
		if (_directives[i]->name() == name)
		{
			if (counter == n)
				return (*(_directives[i]));
			counter++;
		}
	}
	throw(std::runtime_error("No ConfigDirective found."));
}

bool ConfigContext::isConfigValid(const std::map<std::string, bool> &info) const
{
	for (size_t i = 0; i < _directives.size(); i++)
	{
		const std::map<std::string, bool>::const_iterator it
			= info.find(_directives[i]->name());
		if (it == info.end())
			return (false);
		if (it->second != _directives[i]->is_context())
			return (false);
	}
	return (true);
}

bool ConfigContext::hasDirective(const std::string &name) const
{
	return (countDirectivesByName(name) > 0);
}

std::ostream &operator<<(std::ostream &os, const ConfigDirective &directive)
{
	os << "ConfigDirective " << directive.name() << " (";
	for (size_t i = 0; i < directive.nParameters(); i++)
	{
		os << directive.parameter(i);
		if (i != directive.nParameters() - 1)
			os << ", ";
	}
	os << ")";
	return (os);
}

std::ostream &operator<<(std::ostream &os, const ConfigContext &context)
{
	os << "ConfigContext " << context.name() << " (";
	for (size_t i = 0; i < context.nParameters(); i++)
	{
		os << context.parameter(i);
		if (i != context.nParameters() - 1)
			os << ", ";
	}
	os << "), {\n";
	for (size_t i = 0; i < context.nDirectives(); i++)
	{
		if (context.directive(i).is_context())
			os << static_cast<const ConfigContext &>(context.directive(i));
		else
			os << context.directive(i);
		if (i != context.nDirectives() - 1)
			os << ", \n";
	}
	os << "\n}";
	return (os);
}
