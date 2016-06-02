#include "processor.hpp"

#include <iostream>
#include <cassert>

Processor::Processor(Rules rules) :
rules(rules)
{
}

void Processor::process()
{

    std::string input = "";

    while (true) {

        // Read byte and stop if end of stream is reached
        int byte = std::cin.get();
        if (std::cin.eof()) {
            break;
        }
        assert(byte >= 0 && byte < 256);

        input += (char)byte;

        // White space
        if (input == " " || input == "\n" || input == "\t") {
            std::cout << input;
            input = "";

            continue;
        }

        // Comment
        if (input == "--") {
            std::cout << input;
            input = "";

            readComment();

            continue;
        }

        // Multi line comment
        if (input == "/*") {
            std::cout << input;
            input = "";

            readMultilineComment();

            continue;
        }

        // Semicolon
// TODO: Is there a problem if rogue semicolon is met?
        if (input == ";") {
            std::cout << input;
            input = "";

            continue;
        }

    }

}

void Processor::readComment()
{
    // Skip until end of line is reached
    while (true) {
        int byte = std::cin.get();
        if (std::cin.eof()) {
            break;
        }
        assert(byte >= 0 && byte < 256);

        std::cout << (char)byte;

        if (byte == '\n') {
            break;
        }
    }
}

void Processor::readMultilineComment()
{
    char previous_byte = 0;

    // Skip until end of comment is reached
    while (true) {
        int byte = std::cin.get();
        if (std::cin.eof()) {
            break;
        }
        assert(byte >= 0 && byte < 256);

        std::cout << (char)byte;

        if (previous_byte == '*' && byte == '/') {
            break;
        }

        previous_byte = byte;
    }
}
