#include "rules.hpp"

#include <cstddef>
#include <cstdlib>
#include <stdexcept>

Rules::Rules(int argc, char** argv) :
re_delete("^delete\\w*(\\w*)$", REG_EXTENDED)
{
	for (int arg_i = 1; arg_i < argc; ++ arg_i) {
		std::string arg = argv[arg_i];

		int eq_pos = arg.find('=');
		if (eq_pos < 0) {
			throw std::runtime_error("Invalid rule \"" + arg + "\"!");
		}
		std::string table_and_col = arg.substr(0, eq_pos);
		std::string rule = arg.substr(eq_pos+1);

		int dot_pos = table_and_col.find('.');

		// Special rule of deleting whole row
		if (rule == "delete()") {
			if (dot_pos >= 0) {
				throw std::runtime_error("Invalid table rule \"" + rule + "\"!");
			}
			tables[table_and_col].delete_all_rows = true;
			continue;
		}

		// Normal rule. Parse equation
		if (dot_pos < 0) {
			throw std::runtime_error("Invalid column rule \"" + rule + "\"!");
		}
		std::string table = table_and_col.substr(0, dot_pos);
		std::string col = table_and_col.substr(dot_pos+1);
		tables[table].rules_by_names[col] = Equation(rule);
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

std::string Rules::getModifiedValue(std::string const& table_name, unsigned int col_index, std::string const& value, Equation::Variables const& vars)
{
	Tables::const_iterator tables_find = tables.find(table_name);
	if (tables_find == tables.end()) {
		return value;
	}

	RulesByNumber::const_iterator cols_find = tables_find->second.rules_by_indexes.find(col_index);
	if (cols_find == tables_find->second.rules_by_indexes.end()) {
		return value;
	}

	Equation const& rule = cols_find->second;

	return rule.evaluate(vars).toPrintable();
}

bool Rules::getDeleteAllRows(std::string const& table_name) const
{
	Tables::const_iterator tables_find = tables.find(table_name);
	if (tables_find == tables.end()) {
		return false;
	}

	return tables_find->second.delete_all_rows;
}
