#ifndef __PROCESSOR_HPP__
#define __PROCESSOR_HPP__

#include "rules.hpp"

class Processor
{

public:

	Processor(Rules rules);

	void process();

private:

	Rules rules;

	void readComment();

	void readMultilineComment();

};

#endif
