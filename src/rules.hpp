#ifndef __RULES_HPP__
#define __RULES_HPP__

#include <map>
#include <string>

class Rules
{

public:

	Rules(int argc, char** argv);

	std::string getRule(std::string table, std::string col) const;

private:

	typedef std::map<std::string, std::string> RulesMap;

	RulesMap rules;

};

#endif
