#include "processor.hpp"

#include "utils.hpp"

#include <cassert>
#include <cstring>
#include <iostream>
#include <stdexcept>

Processor::Processor(Rules rules) :
rules(rules),
read_buffer_left(0)
{
	read_buffer = new char[READ_BUFFER_SIZE];
}

Processor::~Processor()
{
	delete[] read_buffer;
}

void Processor::process()
{
	while (read_buffer_left > 0 || !std::cin.eof()) {

		fillReadBuffer();
		if (read_buffer_left == 0) {
			continue;
		}

		// White space
		if (isWhitespace(read_buffer[0])) {
			skipWhitespace();
			continue;
		}

		// Comment
		if (read_buffer_left > 2 && strncmp(read_buffer, "--", 2) == 0) {
			skipAndPrint(2);
			skipAndPrintUntil("\n");
			continue;
		}

		// Multi line comment
		if (read_buffer_left > 2 && strncmp(read_buffer, "/*", 2) == 0) {
			skipAndPrint(2);
			skipAndPrintUntil("*/");
			continue;
		}

		// Semicolon
		if (read_buffer[0] == ';') {
			skipAndPrint(1);
			continue;
		}

		throw std::runtime_error("Unable to process input \"" + std::string(read_buffer, std::min(40, read_buffer_left)) + "\"!");
	}
}

void Processor::fillReadBuffer()
{
	std::cin.read(read_buffer + read_buffer_left, READ_BUFFER_SIZE - read_buffer_left);
	read_buffer_left += std::cin.gcount();
}

void Processor::skipWhitespace()
{
	int whitespace_to_skip = 0;
	while (whitespace_to_skip < read_buffer_left) {
		if (!isWhitespace(read_buffer[whitespace_to_skip])) {
			break;
		}
		++ whitespace_to_skip;
	}

	skipAndPrint(whitespace_to_skip);
}

void Processor::skipAndPrint(int amount)
{
	assert(amount <= read_buffer_left);
	std::cout.write(read_buffer, amount);
	std::memmove(read_buffer, read_buffer + amount, read_buffer_left - amount);
	read_buffer_left -= amount;
}

void Processor::skipAndPrintUntil(std::string const& pattern)
{
	bool pattern_found = false;
	while (!pattern_found && !std::cin.eof()) {
		// Fill buffer
		if (read_buffer_left < pattern.size()) {
			fillReadBuffer();
			if (read_buffer_left < pattern.size()) {
				assert(std::cin.eof());
				skipAndPrint(read_buffer_left);
				return;
			}
		}

		// Check how much to skip
		int skip_amount = 0;
		while (skip_amount < read_buffer_left - pattern.size()) {
			if (strncmp(read_buffer + skip_amount, pattern.c_str(), pattern.size()) == 0) {
				pattern_found = true;
				skip_amount += pattern.size();
				break;
			}
			++ skip_amount;
		}

		skipAndPrint(skip_amount);
	}
}
