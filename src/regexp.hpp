#ifndef __REGEXP_HPP__
#define __REGEXP_HPP__

#include <regex.h> 
#include <stdexcept>

#include <cassert> // DEBUG
class Regexp
{

public:

	inline Regexp(std::string const& re, int flags = 0) :
	match_ofs(-1),
	match_len(-1)
	{
		if (regcomp(&this->re, re.c_str(), flags)) {
			throw std::runtime_error("Invalid regular expression \"" + re + "\"!");
		}
	}

	inline ~Regexp()
	{
		regfree(&re);
	}

	inline bool match(std::string const& str)
	{
		regmatch_t match_details;
		int result = regexec(&re, str.c_str(), 1, &match_details, 0);
		if (!result) {
			match_ofs = match_details.rm_so;
			match_len = match_details.rm_eo - match_ofs;
			return true;
		}

		if (result == REG_NOMATCH) {
			match_ofs = -1;
			match_len = -1;
			return false;
		}

		throw std::runtime_error("Unable to check matching of regular expression!");
	}

	inline int getMatchOffset() const
	{
		if (match_ofs < 0) {
			throw std::runtime_error("No match to get offset from!");
		}
		return match_ofs;
	}

	inline int getMatchLength() const
	{
		if (match_len < 0) {
			throw std::runtime_error("No match to get length from!");
		}
		return match_len;
	}

private:

	regex_t re;

	int match_ofs;
	int match_len;

};

#endif
