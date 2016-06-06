#ifndef __REGEXP_HPP__
#define __REGEXP_HPP__

#include "buffer.hpp"

#include <regex.h> 
#include <stdexcept>

class Regexp
{

public:

	inline Regexp(std::string const& pattern, int flags = 0) :
	pattern(pattern),
	flags(flags),
	match_ofs(-1),
	match_len(-1)
	{
		if (regcomp(&this->re, pattern.c_str(), flags)) {
			throw std::runtime_error("Invalid regular expression \"" + pattern + "\"!");
		}
	}

	inline Regexp(Regexp const& re) :
	pattern(re.pattern),
	flags(re.flags),
	match_ofs(re.match_ofs),
	match_len(re.match_len)
	{
		if (regcomp(&this->re, re.pattern.c_str(), re.flags)) {
			throw std::runtime_error("Invalid regular expression \"" + pattern + "\"!");
		}
	}

	inline Regexp& operator=(Regexp const& re)
	{
		regfree(&this->re);
		pattern = re.pattern;
		flags = re.flags;
		match_ofs = re.match_ofs;
		match_len = re.match_len;
		if (regcomp(&this->re, re.pattern.c_str(), re.flags)) {
			throw std::runtime_error("Invalid regular expression \"" + pattern + "\"!");
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

	inline bool match(Buffer& buf)
	{
		return match(buf.peekFull());
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

	std::string pattern;
	int flags;

	regex_t re;

	int match_ofs;
	int match_len;
};

#endif
