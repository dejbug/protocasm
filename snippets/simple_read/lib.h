#ifndef _LIB_H_
#define _LIB_H_

#include <stdio.h>
#include <stdarg.h>
#include <stdexcept>
#include <string>

#include "common.hpp"

namespace lib {

namespace typ {

typedef long i4;
typedef long long i8;
typedef long s4;
typedef long long s8;
typedef unsigned long u4;
typedef unsigned long long u8;

typedef i4 i32;
typedef i8 i64;
typedef s4 s32;
typedef s8 s64;
typedef u4 u32;
typedef u8 u64;

struct bytes
{
	char * data = nullptr;
	size_t capacity = 0;
	size_t good = 0;

	bytes();
	bytes(size_t capacity);
	virtual ~bytes();

	bytes(bytes && other);

	void read(FILE * file, size_t size);
	void read(FILE * file);
};

typedef std::string string;

} // namespace typ

namespace io {

template<size_t N>
size_t read(FILE * file, char (&buffer)[N])
{
	size_t const good = fread(buffer, sizeof(char), N, file);
	if (good < N && !feof(file))
		throw common::make_error("common::read : error while reading from file at %08X: only %d of %d bytes read", (size_t) file, good, N);
	return good;
}

size_t read(FILE * file, char * buffer, size_t size);

template<size_t N>
void dump(char const (&buffer)[N])
{
	dump(buffer, N);
}

void dump(char const * buffer, size_t size);
void dump(lib::typ::bytes & bytes);

struct file
{
	FILE * handle = nullptr;
	char * path = nullptr;

	file(char const * path);
	virtual ~file();

	operator FILE * () { return handle; }
};

lib::typ::bytes read(FILE * file, size_t size);
lib::typ::bytes read(FILE * file);
void read(FILE * file, lib::typ::i4 & out);
void read(FILE * file, lib::typ::i8 & out);

} // namespace io

namespace trans {

lib::typ::i4 flip(lib::typ::i4 value);
lib::typ::i8 flip(lib::typ::i8 value);

} // namespace trans

} // namespace lib

#endif // _LIB_H_
