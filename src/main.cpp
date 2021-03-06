#include "rules.hpp"
#include "processor.hpp"

#include <cstdlib>
#include <iostream>

int main(int argc, char** argv)
{
	srand(time(NULL));

	Rules rules(argc, argv);

	Processor processor(rules);

	processor.process();

	return EXIT_SUCCESS;
}
