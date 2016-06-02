#include "rules.hpp"

#include <stdexcept>
#include <cstddef>

Rules::Rules(int argc, char** argv)
{
	for (int arg_i = 1; arg_i < argc; ++ arg_i) {
		std::string arg = argv[arg_i];

		int eq_pos = arg.find('=');
		if (eq_pos < 0) {
			throw std::runtime_error("Invalid rule \"" + arg + "\"!");
		}
		std::string table_and_col = arg.substr(0, eq_pos);
		std::string rule = arg.substr(eq_pos+1);

		rules[table_and_col] = rule;
	}
}

std::string Rules::getRule(std::string table, std::string col) const
{
	RulesMap::const_iterator rules_find;
	rules_find = rules.find(table + "." + col);

	if (rules_find == rules.end()) {
		return NULL;
	}

	return rules_find->second;
}
