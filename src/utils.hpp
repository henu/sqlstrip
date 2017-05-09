#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include <algorithm>
#include <cstdlib>
#include <sstream>
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

inline bool isLetter(char c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
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

template <typename T> std::string toString (T value)
{
	std::ostringstream ss;
	ss << value;
	return ss.str();
}

inline int randomInteger(int min, int max)
{
	return rand() % (max - min + 1) + min;
}

inline double randomDouble(double min, double max)
{
	return min + double(double(rand()) / RAND_MAX * (max - min));
}

inline char randomChar(std::string const& choices)
{
	unsigned choice = rand() % choices.size();
	return choices[choice];
}

std::string randomString(unsigned min_len, unsigned max_len);

std::string randomWord(unsigned min_len, unsigned max_len);

std::string slashEscape(std::string const& s);

#endif
