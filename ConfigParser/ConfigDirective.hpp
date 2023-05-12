#ifndef CONFIGDIRECTIVE_HPP
#define CONFIGDIRECTIVE_HPP

#include <string>
#include <vector>

class ConfigDirective
{
  private:
	std::string _name;
	std::vector<std::string> _parameters;
	ConfigDirective(void);

  public:
	ConfigDirective(const std::string &name,
					const std::vector<std::string> &parameters);
	~ConfigDirective();
	ConfigDirective(const ConfigDirective &orig);
	ConfigDirective &operator=(const ConfigDirective &orig);
};

class ConfigContext : public ConfigDirective
{
  private:
	std::vector<ConfigDirective *> _directives;
	ConfigContext(void);

  public:
	ConfigContext(const std::string &name,
				  const std::vector<ConfigDirective> &directives);
	~ConfigContext();
	ConfigContext(const ConfigContext &orig);
	ConfigContext &operator=(const ConfigContext &orig);
};

#endif
