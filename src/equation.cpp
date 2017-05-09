#include "equation.hpp"

#include "utils.hpp"

#include <cctype>
#include <cstdio>
#include <stdexcept>

Equation::Equation() :
type(NOTHING)
{
}

Equation::Equation(std::string const& s)
{
	std::string::const_iterator it = s.begin();
	read(it, s.end());
	if (it != s.end()) {
		throw std::runtime_error("Too much data to parse: \"" + std::string(it, s.end()) + "\"!");
	}
}

Equation::Equation(std::string::const_iterator& it, std::string::const_iterator const& end)
{
	read(it, end);
}

Equation Equation::evaluate(Variables const& vars) const
{
	Equation result;

	switch (type) {
	case NUMBER:
	case STRING:
	case CONSTANT_NULL:
		return *this;
	case FUNCTION_DECIMALS:
		{
			result.type = STRING;
			if (children.size() != 2) {
				throw std::runtime_error("decimals() takes exactly two arguments!");
			}
			std::string number = children[0].evaluate(vars).toString();
			int decimals = children[1].evaluate(vars).toInteger();
			std::string::size_type dot_pos = number.find('.');
			if (dot_pos == std::string::npos) {
				if (decimals <= 0) {
					result.str = number;
				} else {
					result.str = number + '.';
					for (int i = 0; i < decimals; ++ i) {
						result.str += '0';
					}
				}
			} else {
				if (decimals <= 0) {
					result.str = number.substr(0, dot_pos);
				} else {
					unsigned correct_len = dot_pos + 1 + decimals;
					// Drop extra numbers
// TODO: Round!
					number = number.substr(0, correct_len);
					while (number.size() < correct_len) {
						number += '0';
					}
					result.str = number;
				}
			}
		}
		break;

	case FUNCTION_IF:
		{
			if (children.size() != 3) {
				throw std::runtime_error("if() takes exactly three arguments!");
			}
			if (children[0].evaluate(vars).toBoolean()) {
				return children[1].evaluate(vars);
			} else {
				return children[2].evaluate(vars);
			}
		}

	case FUNCTION_RANDOM_NUMBER:
		{
			if (children.size() != 2) {
				throw std::runtime_error("random_number() takes exactly two arguments!");
			}
			result.type = NUMBER;
			double min = children[0].evaluate(vars).toDouble();
			double max = children[1].evaluate(vars).toDouble();
			result.number = randomDouble(min, max);
			break;
		}

	case FUNCTION_RANDOM_STRING:
		{
			if (children.size() == 0) {
				throw std::runtime_error("Not enough arguments for random_string()!");
			}
			if (children.size() > 2) {
				throw std::runtime_error("Too many arguments for random_string()!");
			}
			result.type = STRING;
			unsigned min_len = children[0].evaluate(vars).toInteger();
			unsigned max_len = children.size() > 1 ? children[1].evaluate(vars).toInteger() : min_len;
			result.str = randomString(min_len, max_len);
			break;
		}

	case FUNCTION_RANDOM_WORDS:
		{
			if (children.size() == 0) {
				throw std::runtime_error("Not enough arguments for random_words()!");
			}
			if (children.size() > 2) {
				throw std::runtime_error("Too many arguments for random_words()!");
			}
			result.type = STRING;
			unsigned min_words = children[0].evaluate(vars).toInteger();
			unsigned max_words = children.size() > 1 ? children[1].evaluate(vars).toInteger() : min_words;
			unsigned words = randomInteger(min_words, max_words);
			for (unsigned i = 0; i < words; ++ i) {
				if (!result.str.empty()) {
					result.str += " ";
				}
				result.str += randomWord(4, 8);
			}
			break;
		}

	case FUNCTION_RANDOM_EMAIL:
		result.type = STRING;
		result.str = randomWord(4, 8) + "@" + randomWord(4, 8) + "." + randomWord(2, 3);
		if (rand() % 2) {
			result.str = randomWord(4, 8) + "." + result.str;
		}
		break;

	case FUNCTION_RANDOM_PHONENUMBER:
		result.type = STRING;
		result.str = "+";
		for (unsigned i = 0; i < 10; ++ i) {
			result.str += '0' + (rand() % 10);
		}
		break;

	case VARIABLE_NAME:
		{
			Variables::const_iterator vars_find = vars.find(str);
			if (vars_find == vars.end()) {
// TODO: Remove this when variables work!
Equation n;
n.type = CONSTANT_NULL;
return n;
				throw std::runtime_error("Variable \"" + str + "\" is not set!");
			}
			return vars_find->second;
		}
		break;

	default:
		throw std::runtime_error("Unable to evaluate equation!");
	}

	return result;
}

bool Equation::toBoolean() const
{
	switch (type) {
	case NUMBER:
		return number != 0;

	case STRING:
		return !str.empty();

	case CONSTANT_NULL:
		return false;

	default:
		throw std::runtime_error("Unable to convert value to boolean!");
	}
}

long Equation::toInteger() const
{
	switch (type) {
	case NUMBER:
		return long(number + 0.5);

	case STRING:
		throw std::runtime_error("toInteger() is not yet implemented for string types!");

	case CONSTANT_NULL:
		return 0;

	default:
		throw std::runtime_error("Unable to convert value to integer!");
	}
}

double Equation::toDouble() const
{
	switch (type) {
	case NUMBER:
		return number;

	case STRING:
		throw std::runtime_error("toDouble() is not yet implemented for string types!");

	case CONSTANT_NULL:
		return 0;

	default:
		throw std::runtime_error("Unable to convert value to floating point number!");
	}
}

std::string Equation::toString() const
{
	switch (type) {
	case NUMBER:
		return ::toString(number);

	case STRING:
		return str;

	case CONSTANT_NULL:
		return "NULL";

	default:
		throw std::runtime_error("Unable to convert value to string!");
	}
}

std::string Equation::toPrintable() const
{
	switch (type) {
	case NUMBER:
		return ::toString(number);

	case STRING:
		return "\"" + slashEscape(str) + "\"";

	case CONSTANT_NULL:
		return "NULL";

	default:
		throw std::runtime_error("Unable to convert value to printable!");
	}
}

void Equation::read(std::string::const_iterator& it, std::string::const_iterator const& end)
{
	if (it == end) {
		throw std::runtime_error("Empty equation!");
	}

	// Variable, constant or function
	if (isLetter(*it) || *it == '_') {
		std::string name(1, *it);
		++ it;
		while (it != end && (isLetter(*it) || isdigit(*it) || *it == '_')) {
			name += *it;
			++ it;
		}
		skipWhitespace(it, end);

		// If not function
		if (it == end || *it != '(') {
			if (name == "NULL") {
				type = CONSTANT_NULL;
			}
// TODO: Check reserved names here!
			else {
				type = VARIABLE_NAME;
				str = name;
			}
			return;
		}

		// Check function name
		if (name == "decimals") {
			type = FUNCTION_DECIMALS;
		} else if (name == "if") {
			type = FUNCTION_IF;
		} else if (name == "random_number") {
			type = FUNCTION_RANDOM_NUMBER;
		} else if (name == "random_string") {
			type = FUNCTION_RANDOM_STRING;
		} else if (name == "random_words") {
			type = FUNCTION_RANDOM_WORDS;
		} else if (name == "random_email") {
			type = FUNCTION_RANDOM_EMAIL;
		} else if (name == "random_phonenumber") {
			type = FUNCTION_RANDOM_PHONENUMBER;
		} else {
			throw std::runtime_error("Unknown function \"" + name + "\"!");
		}

		// Read arguments of function
		if (it == end || *it != '(') {
			throw std::runtime_error("Missing opening bracket after function \"" + name + "\"!");
		}
		++ it;

		skipWhitespace(it, end);

		if (it == end) {
			throw std::runtime_error("Missing closing bracket in function \"" + name + "\"!");
		}

		if (*it == ')') {
			++ it;
		} else {
			while (true) {
				children.push_back(Equation(it, end));
				skipWhitespace(it, end);

				if (it == end) {
					throw std::runtime_error("Missing closing bracket in function \"" + name + "\"!");
				} else if (*it == ')') {
					++ it;
					break;
				} else if (*it != ',') {
					throw std::runtime_error("Missing comma in function \"" + name + "\"!");
				}
				++ it;
				skipWhitespace(it, end);
			}
		}

		return;
	}

	// Number
	if (isdigit(*it) || *it == '.') {
		// Read as string first
		bool dot_found = (*it == '.');
		std::string number_s(1, *it);
		++ it;
		while (it != end) {
			if (*it == '.') {
				if (dot_found) {
					throw std::runtime_error("Duplicate decimal dot!");
				}
				number_s += *it;
				++ it;
			} else if (isdigit(*it)) {
				number_s += *it;
				++ it;
			} else {
				break;
			}
		}
		// Convert string to double and set type.
		sscanf(number_s.c_str(), "%lf", &number);
		type = NUMBER;
		return;
	}

	// String
	if (*it == '"' || *it == '\'') {
		char str_begin = *it;
		++ it;

		while (true) {
			// Check closing of string
			if (it == end) {
				throw std::runtime_error("String not closed!");
			}
			if (*it == str_begin) {
				++ it;
				break;
			}
			// Special characters
			if (*it == '\\') {
				++ it;
				if (it == end) {
					throw std::runtime_error("Unexpected end for string escaping!");
				}
				if (*it == 'n') {
					str += '\n';
				} else if (*it == 't') {
					str += '\t';
				} else if (*it == '"') {
					str += '"';
				} else if (*it == '\'') {
					str += '\'';
				} else if (*it == '\\') {
					str += '\\';
				} else {
					throw std::runtime_error("Invalding escaping \"" + std::string(it, end) + "\"!");
				}
				++ it;
			}
			// Normal characters
			else {
				str += *it;
				++ it;
			}
		}

		type = STRING;
		return;
	}

	throw std::runtime_error("Unable to read \"" + std::string(it, end) + "\"!");
}

void Equation::skipWhitespace(std::string::const_iterator& it, std::string::const_iterator const& end)
{
	while (it != end && isWhitespace(*it)) {
		++ it;
	}
}
