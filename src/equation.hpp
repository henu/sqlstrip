#ifndef __EQUATION_HPP__
#define __EQUATION_HPP__

#include <string>
#include <map>
#include <vector>

class Equation
{

public:

	typedef std::map<std::string, Equation> Variables;

	Equation();
	Equation(std::string const& s);
	Equation(std::string::const_iterator& it, std::string::const_iterator const& end);

	Equation evaluate(Variables const& vars) const;

	bool toBoolean() const;

	long toInteger() const;

	double toDouble() const;

	std::string toString() const;

	std::string toPrintable() const;

private:

	typedef std::vector<Equation> Equations;

	enum Type {
		NOTHING,
// TODO: Make own type for integers!
		NUMBER,
		STRING,
		CONSTANT_NULL,
		FUNCTION_DECIMALS,
		FUNCTION_IF,
		FUNCTION_RANDOM_NUMBER,
		FUNCTION_RANDOM_STRING,
		FUNCTION_RANDOM_WORDS,
		FUNCTION_RANDOM_EMAIL,
		FUNCTION_RANDOM_PHONENUMBER,
		VARIABLE_NAME
	};

	Type type;

	// Used by function
	Equations children;

	// Used by numbers
	double number;

	// Used by string and variable name
	std::string str;

	void read(std::string::const_iterator& it, std::string::const_iterator const& end);

	static void skipWhitespace(std::string::const_iterator& it, std::string::const_iterator const& end);
};

#endif

