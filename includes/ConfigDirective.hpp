#ifndef CONFIGDIRECTIVE_HPP
#define CONFIGDIRECTIVE_HPP

#include <map>
#include <ostream>
#include <string>
#include <vector>

class ConfigDirective
{
  protected:
	std::string _name;
	std::vector<std::string> _parameters;
	bool _is_context;

  public:
	class InvalidDirective : public std::runtime_error
	{
	  public:
		InvalidDirective(const std::string &why, const ConfigDirective &dir);
		InvalidDirective(const std::string &why);
	};
	class UndefinedDirective : public InvalidDirective
	{
	  public:
		UndefinedDirective(const ConfigDirective &dir);
	};
	class UndefinedArgument : public InvalidDirective
	{
	  public:
		UndefinedArgument(const ConfigDirective &dir);
	};
	class InvalidNumberOfDirective : public InvalidDirective
	{
	  public:
		InvalidNumberOfDirective(const ConfigDirective &dir);
	};
	class InvalidNumberOfArgument : public InvalidDirective
	{
	  public:
		InvalidNumberOfArgument(const ConfigDirective &dir);
	};
	class DuplicateDirective : public InvalidDirective
	{
	  public:
		DuplicateDirective(const ConfigDirective &dir);
	};
	class DuplicateArgument : public InvalidDirective
	{
	  public:
		DuplicateArgument(const ConfigDirective &dir);
	};

	ConfigDirective(const std::string name = "");
	ConfigDirective(const std::string &name,
					const std::vector<std::string> &parameters);
	~ConfigDirective();
	ConfigDirective(const ConfigDirective &orig);
	ConfigDirective &operator=(const ConfigDirective &orig);

	bool is_context(void) const;
	const std::string &name(void) const;
	const std::string &parameter(size_t index) const;
	size_t nParameters(void) const;
};

class ConfigContext : public ConfigDirective
{
  private:
	std::vector<ConfigDirective *> _directives;

  public:
	ConfigContext(const std::string name = "");
	ConfigContext(const std::string &name,
				  const std::vector<std::string> &parameters,
				  const std::vector<ConfigDirective *> &directives);
	~ConfigContext();
	ConfigContext(const ConfigContext &orig);
	ConfigContext &operator=(const ConfigContext &orig);

	const ConfigDirective &directive(size_t index) const;
	size_t nDirectives(void) const;
	size_t countDirectivesByName(const std::string &name) const;
	const ConfigDirective &getNthDirectiveByName(const std::string &name,
												 size_t n) const;
	bool isConfigValid(const std::map<std::string, bool> &info) const;
	bool hasDirective(const std::string &name) const;
};

std::ostream &operator<<(std::ostream &os, const ConfigDirective &directive);
std::ostream &operator<<(std::ostream &os, const ConfigContext &context);

#endif
