#ifndef __PROCESSOR_HPP__
#define __PROCESSOR_HPP__

#include "rules.hpp"

class Processor
{

public:

	Processor(Rules rules);
	~Processor();

	void process();

private:

	Rules rules;

	static int const READ_BUFFER_SIZE = 10000;
	char* read_buffer;
	int read_buffer_left;

	void fillReadBuffer();

	void skipWhitespace();

	void skipAndPrint(int amount);

	void skipAndPrintUntil(std::string const& pattern);
};

#endif
