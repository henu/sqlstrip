#ifndef __BUFFER_HPP__
#define __BUFFER_HPP__

#include <iostream>
#include <cstring>
#include <stdexcept>

class Buffer
{

public:

	inline Buffer() :
	read_buffer_left(0),
	read_buffer_ofs(0)
	{
		read_buffer = new char[READ_BUFFER_SIZE];
	}

	inline ~Buffer()
	{
		delete[] read_buffer;
	}

	// Returns true if there is no more bytes available
	inline bool eof()
	{
		if (read_buffer_left > 0) {
			return false;
		}

		fillBuffer();

		return read_buffer_left == 0;
	}

	inline char peek()
	{
		if (read_buffer_left == 0) {
			fillBuffer();
			if (read_buffer_left == 0) {
				throw std::runtime_error("No bytes left!");
			}
		}
		return read_buffer[read_buffer_ofs];
	}

	// Result might be shorter if there is not enough bytes
	inline std::string peek(unsigned int length)
	{
		if (read_buffer_left < length) {
			fillBuffer();
			if (read_buffer_left == 0) {
				throw std::runtime_error("No bytes left!");
			}
		}

		if (read_buffer_left < length) {
			return std::string(read_buffer, read_buffer_left);
		}

		return std::string(read_buffer, length);
	}

	inline std::string peekFull()
	{
		if (read_buffer_ofs != 0) {
			fillBuffer();
		}

		return std::string(read_buffer, read_buffer_left);
	}

	inline bool isNext(std::string const& str)
	{
		if (read_buffer_left < str.size()) {
			fillBuffer();
			if (read_buffer_left == 0) {
				throw std::runtime_error("No bytes left!");
			}
		}

		if (read_buffer_left < str.size()) {
			return false;
		}

		return std::strncmp(read_buffer + read_buffer_ofs, str.c_str(), str.size()) == 0;
	}

	inline char get()
	{
		char result = getWithoutPrinting();
		std::cout << result;
		return result;
	}

	inline char getWithoutPrinting()
	{
		if (read_buffer_left == 0) {
			fillBuffer();
			if (read_buffer_left == 0) {
				throw std::runtime_error("No bytes left!");
			}
		}

		char result = read_buffer[read_buffer_ofs];
		++ read_buffer_ofs;
		-- read_buffer_left;

		return result;
	}

	inline void skip()
	{
		if (read_buffer_left == 0) {
			fillBuffer();
			if (read_buffer_left == 0) {
				throw std::runtime_error("No bytes left!");
			}
		}

		std::cout << read_buffer[read_buffer_ofs];
		++ read_buffer_ofs;
		-- read_buffer_left;
	}

	inline void skip(unsigned int amount)
	{
		while (amount > 0) {
			if (read_buffer_left == 0) {
				fillBuffer();
				if (read_buffer_left == 0) {
					throw std::runtime_error("No bytes left!");
				}
			}

			unsigned int chunk_size = std::min(amount, read_buffer_left);

			std::cout.write(read_buffer + read_buffer_ofs, chunk_size);
			read_buffer_ofs += chunk_size;
			read_buffer_left -= chunk_size;
			amount -= chunk_size;
		}
	}

private:

	static unsigned int const READ_BUFFER_SIZE = 10000;

	char* read_buffer;
	unsigned int read_buffer_left;
	unsigned int read_buffer_ofs;

	inline void fillBuffer()
	{
		if (read_buffer_left > 0 && read_buffer_ofs == 0) {
			return;
		}

		// Move remaining stuff to the beginning of buffer
		std::memmove(read_buffer, read_buffer + read_buffer_ofs, read_buffer_left);
		read_buffer_ofs = 0;

		// Load more
		unsigned int read_amount = READ_BUFFER_SIZE - read_buffer_left;
		std::cin.read(read_buffer + read_buffer_left, read_amount);
		read_buffer_left += std::cin.gcount();
	}
};

#endif
