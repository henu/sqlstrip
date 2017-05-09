#include "utils.hpp"

#include <cstring>

std::string const VOWELS = "aeiouy";
std::string const CONSONANTS = "bcdfghjklmnpqrstvwxz";
std::string const RANDOM_CHARS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

std::string randomString(unsigned min_len, unsigned max_len)
{
	unsigned len = randomInteger(min_len, max_len);
	std::string s;
	while (s.size() < len) {
		s += randomChar(RANDOM_CHARS);
	}
	return s;
}

std::string randomWord(unsigned min_len, unsigned max_len)
{
	unsigned len = randomInteger(min_len, max_len);
	std::string word;
	bool consonant = true;
	while (word.size() < len) {
		if (consonant) {
			word += randomChar(CONSONANTS);
		} else {
			word += randomChar(VOWELS);
		}
		consonant = !consonant;
	}

	return word;
}

std::string slashEscape(std::string const& s)
{
// TODO: In future, support escaping unicode characters!
	std::string escaped;

	for (std::string::const_iterator it = s.begin(); it != s.end(); ++ it) {
		if (*it == '\n') {
			escaped += "\\n";
		} else if (*it == '\t') {
			escaped += "\\t";
		} else if (*it == '"') {
			escaped += "\\\"";
		} else if (*it == '\'') {
			escaped += "\\'";
		} else if (*it == '\\') {
			escaped += "\\\\";
		} else {
			escaped += *it;
		}
	}

	return escaped;
}
