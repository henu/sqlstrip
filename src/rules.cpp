#include "rules.hpp"

#include <cstddef>
#include <cstdlib>
#include <stdexcept>

char const* RANDOM_CHARS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
unsigned int RANDOM_CHARS_LEN = strlen(RANDOM_CHARS);

Rules::Rules(int argc, char** argv) :
re_random_string("^random_string\\([0-9]+\\)$", REG_EXTENDED),
re_num_arg("\\([0-9]+\\)", REG_EXTENDED)
{
	for (int arg_i = 1; arg_i < argc; ++ arg_i) {
		std::string arg = argv[arg_i];

		int eq_pos = arg.find('=');
		if (eq_pos < 0) {
			throw std::runtime_error("Invalid rule \"" + arg + "\"!");
		}
		std::string table_and_col = arg.substr(0, eq_pos);
		std::string rule = arg.substr(eq_pos+1);

		if (rule != "empty_string" && !re_random_string.match(rule)) {
			throw std::runtime_error("Unknown rule \"" + rule + "\"!");
		}

		int dot_pos = table_and_col.find('.');
		if (dot_pos < 0) {
			throw std::runtime_error("Invalid table and column definition \"" + table_and_col + "\"!");
		}
		std::string table = table_and_col.substr(0, dot_pos);
		std::string col = table_and_col.substr(dot_pos+1);

		tables[table].rules_by_names[col] = rule;
	}
}

void Rules::setTableColumnNames(std::string const& table_name, Strings const& cols)
{
	Tables::iterator tables_find = tables.find(table_name);
	if (tables_find == tables.end()) {
		return;
	}

	// Now it is possible to set rules by indexes
	Table& table = tables_find->second;
	for (unsigned int col_i = 0; col_i < cols.size(); ++ col_i) {
		RulesByString::iterator rules_find = table.rules_by_names.find(cols[col_i]);
		if (rules_find != table.rules_by_names.end()) {
			table.rules_by_indexes[col_i] = rules_find->second;
		}
	}
}

std::string Rules::getModifiedValue(std::string const& table_name, unsigned int col_index, std::string const& value)
{
	Tables::const_iterator tables_find = tables.find(table_name);
	if (tables_find == tables.end()) {
		return value;
	}

	RulesByNumber::const_iterator cols_find = tables_find->second.rules_by_indexes.find(col_index);
	if (cols_find == tables_find->second.rules_by_indexes.end()) {
		return value;
	}

	std::string const& rule = cols_find->second;

	if (rule == "empty_string") {
		return "''";
	}

	if (re_random_string.match(rule)) {
		re_num_arg.match(rule);
		std::string num_str = rule.substr(re_num_arg.getMatchOffset() + 1, re_num_arg.getMatchLength() - 2);
		int num = std::atoi(num_str.c_str());
		std::string result = "'";
		for (int i = 0; i < num; ++ i) {
			int r = rand() % RANDOM_CHARS_LEN;
			result += RANDOM_CHARS[r];
		}
		result += "'";
		return result;
	}

	throw std::runtime_error("Unknown rule \"" + rule + "\"!");
}
