#include "processor.hpp"

#include "utils.hpp"
#include "regexp.hpp"

#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>
#include <stdexcept>

Processor::Processor(Rules rules) :
rules(rules)
{
}

void Processor::process()
{
	Regexp re_create_table("^CREATE +TABLE\\s", REG_EXTENDED | REG_ICASE);
	Regexp re_insert_into("^INSERT +INTO\\s", REG_EXTENDED | REG_ICASE);
	Regexp re_values("^VALUES\\s", REG_EXTENDED | REG_ICASE);
	Regexp re_ignored("^(DROP|LOCK|UNLOCK) +TABLE", REG_EXTENDED | REG_ICASE);

	while (!buf.eof()) {

		// White space
		if (isWhitespace(buf.peek())) {
			skipWhitespace();
			continue;
		}

		// Comment
		if (buf.isNext("--")) {
			buf.skip(2);
			skipUntil("\n");
			continue;
		}

		// Multi line comment
		if (buf.isNext("/*")) {
			buf.skip(2);
			skipUntil("*/");
			continue;
		}

		// Semicolon
		if (buf.peek() == ';') {
			buf.skip();
			continue;
		}

		// Ignored statement
		if (re_ignored.match(buf)) {
			buf.skip(re_ignored.getMatchLength());
			skipUntil(";");
			continue;
		}

		// CREATE TABLE statement
		if (re_create_table.match(buf)) {
			buf.skip(re_create_table.getMatchLength());

			skipWhitespaceCommentsAndFillBuffer();

			std::string table_name = readName();

			skipWhitespaceCommentsAndFillBuffer();

			if (buf.eof() || buf.get() != '(') {
				throw std::runtime_error("Expected \"(\" was not found!");
			}

			skipWhitespaceCommentsAndFillBuffer();

			// Read names of columns
			Strings cols;
			while (!buf.eof() && buf.peek() != ')') {
				std::string col_name = readName();

				// Make sure this is a column
				if (!col_name.empty()) {
					cols.push_back(col_name);
				}

				// Skip all the details
				int open_brackets = 0;
				while (true) {
					if (buf.eof() || (buf.peek() == ')' && open_brackets == 0)) {
						break;
					}

					char byte = buf.get();
					if (byte == '(') {
						++ open_brackets;
					} else if (byte == ')') {
						if (open_brackets == 0) {
							throw std::runtime_error("Unexpected closing bracket!");
						}
						-- open_brackets;
					} else if (byte == ',' && open_brackets == 0) {
						break;
					}
				}

				skipWhitespaceCommentsAndFillBuffer();
			}

			if (buf.eof() || buf.get() != ')') {
				throw std::runtime_error("Expected \")\" was not found!");
			}

			skipUntil(";");

			rules.setTableColumnNames(table_name, cols);

			continue;
		}

		// INSERT INTO statement
		if (re_insert_into.match(buf)) {
			buf.skip(re_insert_into.getMatchLength());

			skipWhitespaceCommentsAndFillBuffer();

			std::string table_name = readName();

			skipWhitespaceCommentsAndFillBuffer();

			if (!re_values.match(buf)) {
				throw std::runtime_error("Expected \"VALUES\" was not found!");
			}
			buf.skip(re_values.getMatchLength());

			skipWhitespaceCommentsAndFillBuffer();

			// Read inserting of values
			while (true) {
				if (buf.eof() || buf.get() != '(') {
					throw std::runtime_error("Expected \"(\" was not found!");
				}

				skipWhitespaceCommentsAndFillBuffer();

				if (buf.eof() || buf.peek() != ')') {
					unsigned int col_i = 0;
					while (true) {
						std::string value = readValueWithoutPrinting();

						// Let rules modify the value
						value = rules.getModifiedValue(table_name, col_i, value);
						std::cout << value;

						skipWhitespaceCommentsAndFillBuffer();

						if (buf.eof()) {
							break;
						}
						char next_byte = buf.get();
						if (next_byte == ')') {
							break;
						} else if (next_byte != ',') {
							throw std::runtime_error("Expected \"(\" or \",\" was not found!");
						}

						skipWhitespaceCommentsAndFillBuffer();

						++ col_i;
					}
				} else {
					// Skip ')'
					buf.skip();
				}

				skipWhitespaceCommentsAndFillBuffer();

				if (buf.eof()) {
					break;
				}
				char comma_or_semicolon = buf.get();
				if (comma_or_semicolon != ',') {
					if (comma_or_semicolon != ';') {
						throw std::runtime_error("Expected \";\" was not found!");
					}
					break;
				}

				skipWhitespaceCommentsAndFillBuffer();
			}

			continue;
		}

		throw std::runtime_error("Unable to process input \"" + buf.peek(40) + "\"!");
	}
}

void Processor::skipWhitespace()
{
	while (!buf.eof() && isWhitespace(buf.peek())) {
		buf.get();
	}
}

void Processor::skipUntil(std::string const& pattern)
{
	while (!buf.eof()) {
		if (buf.isNext(pattern)) {
			buf.skip(pattern.size());
			return;
		}
		buf.skip();
	}
}

void Processor::skipWhitespaceCommentsAndFillBuffer()
{
	while (!buf.eof()) {

		// White space
		if (isWhitespace(buf.peek())) {
			skipWhitespace();
			continue;
		}

		// Comment
		if (buf.isNext("--")) {
			buf.skip(2);
			skipUntil("\n");
			continue;
		}

		// Multi line comment
		if (buf.isNext("/*")) {
			buf.skip(2);
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
	if (!buf.eof() && buf.peek() == '`') {
		quote_char = buf.peek();
		buf.skip();
	}

	while (!buf.eof()) {
		char next_byte = buf.peek();

		if (quote_char && next_byte == quote_char) {
			buf.skip();
			break;
		}

		if (!quote_char && endsName(next_byte)) {
			break;
		}

		name += buf.get();
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

std::string Processor::readValueWithoutPrinting()
{
	std::string value;

	if (buf.eof()) {
		return "";
	}

	char next_byte = buf.peek();

	// If number
	if ((next_byte >= '0' && next_byte <= '9') || next_byte == '.') {
		value += buf.getWithoutPrinting();

		while (!buf.eof()) {
			next_byte = buf.peek();

			if ((next_byte >= '0' && next_byte <= '9') || next_byte == '.') {
				value += buf.getWithoutPrinting();
				continue;
			}

			break;
		}

		return value;
	}
	// If string
	else if (next_byte == '"' || next_byte == '\'') {
		value += buf.getWithoutPrinting();

		char quote_type = next_byte;

		while (true) {
			if (buf.eof()) {
				throw std::runtime_error("String value is missing the closing quote character!");
			}

			char byte = buf.getWithoutPrinting();

			value += byte;

			if (byte == quote_type) {
				break;
			} else if (byte == '\\') {
				if (buf.eof()) {
					throw std::runtime_error("String value is missing the closing quote character!");
				}
				value += buf.getWithoutPrinting();
			}
		}

		return value;
	}
	// If null
	else if (uppercase(buf.peek(4)) == "NULL") {
		return buf.getWithoutPrinting(4);
	}

	throw std::runtime_error("Unable to parse value!");
}
