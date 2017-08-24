#ifndef _IO_HPP_
#define _IO_HPP_

#include <stdio.h>
#include <assert.h>
#include "common.hpp"

namespace io {

long read32(FILE * file)
{
	assert(sizeof(long) == 4);
	long v = 0;
	common::read(file, (char *) &v, 4);
	return v;
}

long long read64(FILE * file)
{
	assert(sizeof(long long) == 8);
	long long v = 0;
	common::read(file, (char *) &v, 8);
	return v;
}

#define SHL32(a, i) (((unsigned long) (((unsigned char const *) a)[i])) << (24-i*8))
// #define SHL32(a, i, s) ((((unsigned char const *) a)[i]) << (s))

long flip32(long value)
{
	// TODO: ws2_32.dll's htonl() does this much more
	// efficiently, using only scratch registers .

	return SHL32(&value, 0) | SHL32(&value, 1) | SHL32(&value, 2) | SHL32(&value, 3);

	auto x = (unsigned char const *) &value;
	return (long) (unsigned long) (x[0] << 24) | (x[1] << 16) | (x[2] << 8) | x[3];
}

#define SHL64(a, i) (((unsigned long long) (((unsigned char const *) a)[i])) << (56-i*8))

long long flip64(long long value)
{
	return SHL64(&value, 0) | SHL64(&value, 1) | SHL64(&value, 2) | SHL64(&value, 3) | SHL64(&value, 4) | SHL64(&value, 5) | SHL64(&value, 6) | SHL64(&value, 7);
}

long to_int32();
unsigned long to_uint32();
signed long to_sint32();

long long to_int64();
unsigned long long to_uint64();
signed long long to_sint64();

bool to_bool();
int to_enum();

void to_fixed64();
void to_sfixed64();
void to_double();
void to_string();
void to_bytes();
void to_embedded_messages();
void to_packed_repeated_fields();
void to_fixed32();
void to_sfixed32();
void to_float();

} // namespace io

#endif // _IO_HPP_
