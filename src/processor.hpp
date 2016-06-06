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

	void skipWhitespace();

	void skipUntil(std::string const& pattern);

	void skipWhitespaceCommentsAndFillBuffer();

	std::string readName();

	std::string readValueWithoutPrinting();
};

#endif
