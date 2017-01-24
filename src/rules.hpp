#ifndef __RULES_HPP__
#define __RULES_HPP__

#include "regexp.hpp"
#include "types.hpp"

#include <map>
#include <string>

class Rules
{

public:

	Rules(int argc, char** argv);

	void setTableColumnNames(std::string const& table, Strings const& cols);

	std::string getModifiedValue(std::string const& table_name, unsigned int col_index, std::string const& value);

	bool getDeleteAllRows(std::string const& table_name) const;

private:

	typedef std::map<std::string, std::string> RulesByString;
	typedef std::map<unsigned int, std::string> RulesByNumber;

	struct Table
	{
		RulesByString rules_by_names;
		RulesByNumber rules_by_indexes;
		bool delete_all_rows;

		inline Table() : delete_all_rows(false) {}
	};

	typedef std::map<std::string, Table> Tables;

	Regexp re_random_string;
	Regexp re_num_arg;

	Tables tables;

};

#endif
