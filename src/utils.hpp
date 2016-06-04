#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include <algorithm>
#include <string>

inline std::string uppercase(std::string const& str)
{
	std::string result;
	for (unsigned int str_i = 0; str_i < str.size(); ++ str_i) {
		result += std::toupper(str[str_i]);
	}
	return result;
}

inline bool isWhitespace(char c)
{
	return c == ' ' || c == '\n' || c == '\t';
}

inline bool endsName(char c)
{
	if (isWhitespace(c)) {
		return true;
	}

	if (c == '(') {
		return true;
	}

	return false;
}

#endif