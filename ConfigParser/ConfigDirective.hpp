#ifndef CONFIGDIRECTIVE_HPP
#define CONFIGDIRECTIVE_HPP

#include <string>
#include <vector>
#include <ostream>

class ConfigDirective
{
  protected:
	std::string _name;
	std::vector<std::string> _parameters;
	bool _is_context;

  public:
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
};

std::ostream &operator<<(std::ostream &os, const ConfigDirective &directive);
std::ostream &operator<<(std::ostream &os, const ConfigContext &context);

#endif