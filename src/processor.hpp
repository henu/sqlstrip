#ifndef __PROCESSOR_HPP__
#define __PROCESSOR_HPP__

#include "buffer.hpp"
#include "rules.hpp"
#include "types.hpp"

class Processor
{

public:

	Processor(Rules rules);

	void process();

private:

	Rules rules;

	Buffer buf;

	// If target is null, then print to std::cout
	void skipWhitespace(std::string* target = NULL);

	// If target is null, then print to std::cout
	void skipUntil(std::string const& pattern, std::string* target = NULL);

	// If target is null, then print to std::cout
	void skipWhitespaceCommentsAndFillBuffer(std::string* target = NULL);

	// If target is null, then print to std::cout
	std::string readName(std::string* target = NULL);

	std::string readValueWithoutPrinting();
};

#endif
