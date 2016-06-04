#ifndef __PROCESSOR_HPP__
#define __PROCESSOR_HPP__

#include "rules.hpp"
#include "types.hpp"

#include <map>

class Processor
{

public:

	Processor(Rules rules);
	~Processor();

	void process();

private:

	typedef std::map<std::string, Strings> ColsOfTables;

	Rules rules;

	static int const READ_BUFFER_SIZE = 10000;
	char* read_buffer;
	unsigned int read_buffer_left;

	void fillReadBuffer();

	void skipWhitespace();

	void skip(unsigned int amount);

	void skipUntil(std::string const& pattern);

	void skipWhitespaceCommentsAndFillBuffer();

	std::string readName();

	// Returns negative if end of file was met
	int peekNextByte();

	// Returns negative if end of file was met
	int readNextByte();

	// Returns negative if end of file was met
	int readNextByteWithoutPrinting();

	// Returns true if at least one whitespace was found
	bool readWhitespace();

	std::string readValueWithoutPrinting();
};

#endif
