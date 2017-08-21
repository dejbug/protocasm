#ifndef _OPERATIONS_H_
#define _OPERATIONS_H_

#include <stdio.h>
#include <stdarg.h>
#include <stdexcept>
#include <string>

#include "common.hpp"

namespace op {

size_t read(FILE * file, char * buffer, size_t size);
void dump(char const * buffer, size_t size);
unsigned long flip_32(unsigned long value);
unsigned long long flip_64(unsigned long long value);
unsigned long read_fixed32(FILE * file);
unsigned long long read_fixed64(FILE * file);
int read_varint_32(FILE * file);
std::string read_string(FILE * file);

template<size_t N>
size_t read_varint(FILE * file, char (&buffer)[N])
{
	for (size_t i=0; i < N; ++i)
	{
		char byte;
		read(file, &byte, 1);
		buffer[i] = 0x7F & byte;
		if ((byte & 0x80) == 0)
			return i + 1;
	}
	throw make_error("op::read_varint : end of buffer reached at byte %d before varint fully read", N);
}

struct Mark
{
	FILE * file;
	long mark = 0;

	Mark(FILE * file);
	bool operator<(size_t pos) const;
	bool operator>(size_t pos) const;
	bool operator>=(size_t pos) const;

	void set();
	size_t get_dist() const;
};

struct Key
{
	int id = 0;		// field id
	int wt = -1;	// wire type

	Key(int id, int wt);
};

struct Varint
{
	char data[32] = {0};
	size_t good = 0;

	void read(FILE * file);

	Key to_key() const;

	long to_int32() const;
	long long to_int64() const;
	signed long to_sint32() const;
	signed long long to_sint64() const;
	unsigned long to_uint32() const;
	unsigned long long to_uint64() const;
	bool to_bool() const;
	int to_enum() const;
};

} // namespace op

#endif // _OPERATIONS_H_
