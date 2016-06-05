#include "processor.hpp"

#include "utils.hpp"
#include "regexp.hpp"

#include <algorithm>
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
	Regexp re_create_table("^CREATE +TABLE\\s", REG_EXTENDED | REG_ICASE);
	Regexp re_insert_into("^INSERT +INTO\\s", REG_EXTENDED | REG_ICASE);
	Regexp re_values("^VALUES\\s", REG_EXTENDED | REG_ICASE);
	Regexp re_ignored("^(DROP|LOCK|UNLOCK) +TABLE", REG_EXTENDED | REG_ICASE);

	ColsOfTables cols_of_tables;

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
			skip(2);
			skipUntil("\n");
			continue;
		}

		// Multi line comment
		if (read_buffer_left > 2 && strncmp(read_buffer, "/*", 2) == 0) {
			skip(2);
			skipUntil("*/");
			continue;
		}

		// Semicolon
		if (read_buffer[0] == ';') {
			skip(1);
			continue;
		}

		std::string read_buffer_str(read_buffer, read_buffer_left);

		// Ignored statement
		if (re_ignored.match(read_buffer_str)) {
			skipUntil(";");
			continue;
		}

		// CREATE TABLE statement
		if (re_create_table.match(read_buffer_str)) {
			skip(re_create_table.getMatchLength());

			skipWhitespaceCommentsAndFillBuffer();

			std::string table_name = readName();

			skipWhitespaceCommentsAndFillBuffer();

			if (readNextByte() != '(') {
				throw std::runtime_error("Expected \"(\" was not found!");
			}

			skipWhitespaceCommentsAndFillBuffer();

			// Read names of columns
			Strings cols;
			while (peekNextByte() >= 0 && peekNextByte() != ')') {
				std::string col_name = readName();

				// Make sure this is a column
				if (!col_name.empty()) {
					cols.push_back(col_name);
				}

				// Skip all the details
				int open_brackets = 0;
				while (true) {
					int next_byte = peekNextByte();
					if (next_byte < 0 || (next_byte == ')' && open_brackets == 0)) {
						break;
					}

// TODO: Optimize!
					int byte = readNextByte();
					assert(byte >= 0);
					if (byte == '(') {
						++ open_brackets;
					} else if (byte == ')') {
						-- open_brackets;
						assert(open_brackets >= 0);
					} else if (byte == ',' && open_brackets == 0) {
						break;
					}
				}

				skipWhitespaceCommentsAndFillBuffer();
			}

			if (readNextByte() != ')') {
				throw std::runtime_error("Expected \")\" was not found!");
			}

			skipUntil(";");

			rules.setTableColumnNames(table_name, cols);

			cols_of_tables[table_name] = cols;

			continue;
		}

		// INSERT INTO statement
		if (re_insert_into.match(read_buffer_str)) {
			skip(re_insert_into.getMatchLength());

			skipWhitespaceCommentsAndFillBuffer();

			std::string table_name = readName();

			skipWhitespaceCommentsAndFillBuffer();

			read_buffer_str = std::string(read_buffer, read_buffer_left);
			if (!re_values.match(read_buffer_str)) {
				throw std::runtime_error("Expected \"VALUES\" was not found!");
			}
			skip(re_values.getMatchLength());

			skipWhitespaceCommentsAndFillBuffer();

			// Read inserting of values
			while (true) {
				if (readNextByte() != '(') {
					throw std::runtime_error("Expected \"(\" was not found!");
				}

				skipWhitespaceCommentsAndFillBuffer();

				if (peekNextByte() != ')') {
					unsigned int col_i = 0;
					while (true) {
						std::string value = readValueWithoutPrinting();

						// Let rules modify the value
						value = rules.getModifiedValue(table_name, col_i, value);
						std::cout << value;

						skipWhitespaceCommentsAndFillBuffer();

						int next_byte = readNextByte();
						if (next_byte == ')' || next_byte < 0) {
							break;
						} else if (next_byte != ',') {
							throw std::runtime_error("Expected \"(\" or \",\" was not found!");
						}

						skipWhitespaceCommentsAndFillBuffer();

						++ col_i;
					}
				} else {
					// Skip ')'
					readNextByte();
				}


				skipWhitespaceCommentsAndFillBuffer();

				int comma_or_semicolon = readNextByte();
				if (comma_or_semicolon != ',') {
					if (comma_or_semicolon >= 0 && comma_or_semicolon != ';') {
						throw std::runtime_error("Expected \";\" was not found!");
					}
					break;
				}

				skipWhitespaceCommentsAndFillBuffer();
			}

			continue;
		}

		throw std::runtime_error("Unable to process input \"" + std::string(read_buffer, std::min<unsigned int>(40, read_buffer_left)) + "\"!");
	}
}

void Processor::fillReadBuffer()
{
	std::cin.read(read_buffer + read_buffer_left, READ_BUFFER_SIZE - read_buffer_left);
	read_buffer_left += std::cin.gcount();
}

void Processor::skipWhitespace()
{
	unsigned int whitespace_to_skip = 0;
	while (whitespace_to_skip < read_buffer_left) {
		if (!isWhitespace(read_buffer[whitespace_to_skip])) {
			break;
		}
		++ whitespace_to_skip;
	}

	skip(whitespace_to_skip);
}

void Processor::skip(unsigned int amount)
{
	assert(amount <= read_buffer_left);
	std::cout.write(read_buffer, amount);
	std::memmove(read_buffer, read_buffer + amount, read_buffer_left - amount);
	read_buffer_left -= amount;
}

void Processor::skipUntil(std::string const& pattern)
{
	bool pattern_found = false;
	while (!pattern_found && !std::cin.eof()) {
		// Fill buffer
		if (read_buffer_left < pattern.size()) {
			fillReadBuffer();
			if (read_buffer_left < pattern.size()) {
				assert(std::cin.eof());
				skip(read_buffer_left);
				return;
			}
		}

		// Check how much to skip
		unsigned int skip_amount = 0;
		while (skip_amount < read_buffer_left - pattern.size()) {
			if (strncmp(read_buffer + skip_amount, pattern.c_str(), pattern.size()) == 0) {
				pattern_found = true;
				skip_amount += pattern.size();
				break;
			}
			++ skip_amount;
		}

		skip(skip_amount);
	}
}

void Processor::skipWhitespaceCommentsAndFillBuffer()
{
	fillReadBuffer();

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
			skip(2);
			skipUntil("\n");
			continue;
		}

		// Multi line comment
		if (read_buffer_left > 2 && strncmp(read_buffer, "/*", 2) == 0) {
			skip(2);
			skipUntil("*/");
			continue;
		}

		// No whitespace, no comment, everything has been skipped
		break;
	}
}

std::string Processor::readName()
{
	std::string name = "";

	// Check if there is quote char
	char quote_char = 0;
	int next_byte = peekNextByte();
	if (next_byte == '`') {
		quote_char = next_byte;
		skip(1);
	}

	while (true) {
		next_byte = peekNextByte();

		if (quote_char && next_byte == quote_char) {
			skip(1);
			break;
		}

		if (!quote_char && endsName(next_byte)) {
			break;
		}

		int byte = readNextByte();
		if (byte < 0) {
			break;
		}

		name += (char)byte;
	}

	// Make sure the name is not reserved
	if (!quote_char) {
		std::string name_upper = uppercase(name);
		if (name_upper == "PRIMARY" ||
		    name_upper == "UNIQUE" ||
		    name_upper == "KEY" ||
		    name_upper == "CONSTRAINT") {
			return "";
		}
	}

	return name;
}

int Processor::peekNextByte()
{
	while (read_buffer_left == 0) {
		fillReadBuffer();
		if (read_buffer_left == 0 && std::cin.eof()) {
			return -1;
		}
	}

	return (unsigned char)read_buffer[0];
}

int Processor::readNextByte()
{
	while (read_buffer_left == 0) {
		fillReadBuffer();
		if (read_buffer_left == 0 && std::cin.eof()) {
			return -1;
		}
	}

	int byte = read_buffer[0];
	skip(1);

	return (unsigned char)byte;
}

int Processor::readNextByteWithoutPrinting()
{
	while (read_buffer_left == 0) {
		fillReadBuffer();
		if (read_buffer_left == 0 && std::cin.eof()) {
			return -1;
		}
	}

	int byte = read_buffer[0];

	assert(read_buffer_left > 0);
	std::memmove(read_buffer, read_buffer + 1, read_buffer_left - 1);
	-- read_buffer_left;

	return (unsigned char)byte;
}

std::string Processor::readValueWithoutPrinting()
{
	std::string value;

	int next_byte = peekNextByte();

	// If number
	if ((next_byte >= '0' && next_byte <= '9') || next_byte == '.') {
		value += readNextByteWithoutPrinting();

		while (true) {
			next_byte = peekNextByte();

			if ((next_byte >= '0' && next_byte <= '9') || next_byte == '.') {
				value += readNextByteWithoutPrinting();
				continue;
			}

			break;
		}

		return value;
	}

	throw std::runtime_error("Unable to parse value!");
}
