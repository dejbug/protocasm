#ifndef _IO_HPP_
#define _IO_HPP_

#include <stdio.h>
#include <assert.h>
#include "common.hpp"

namespace typ {

typedef long i4;
typedef long long i8;
typedef long s4;
typedef long long s8;
typedef unsigned long u4;
typedef unsigned long long u8;

struct bytes
{
	char * data = nullptr;
	size_t size = 0;
	size_t good = 0;
};

}

namespace buf {

void read(FILE * file, typ::bytes & out, size_t size);
void read(FILE * file, typ::bytes & out);

void read(FILE * file, typ::i4 & out);
void read(FILE * file, typ::i8 & out);

}


namespace io {

struct buffer
{
	char * data = nullptr;
	size_t capacity = 0;
	size_t good = 0;

	buffer(size_t capacity)
		: capacity{capacity}
	{
		if (capacity > 0)
			data = new char[capacity];
	}

	virtual ~buffer()
	{
		capacity = 0;
		if (data)
		{
			delete[] data;
			data = nullptr;
		}
	}

	void read(FILE * file, size_t size)
	{
		good = 0;

		if (size > capacity)
			throw common::make_error("buffer::read : not enough room for reading %d bytes from file at %08X: only %d bytes allocated", size, (size_t) file, capacity);

		good = fread(data, 1, size, file);

		if (good < size && !feof(file))
			throw common::make_error("buffer::read : error while reading from file at %08X: only %d of %d bytes read", (size_t) file, good, size);
	}

	void read(FILE * file)
	{
		good = 0;

		for (size_t i = 0; i < capacity; ++i)
		{
			char byte[1];
			common::read(file, byte);
			data[i] = 0x7F & byte[0];
			if (byte[0] & 0x80) continue;
			good = i + 1;
			return;
		}

		throw common::make_error("buffer::read : end of buffer reached at byte %d before varint fully read", capacity);
	}
};


void dump(char const * buffer, size_t size)
{
	for (size_t i=0; i<size; ++i)
	{
		if (i > 0)
		{
			if(i % 16 == 0) printf("\n");
			else if(i % 4 == 0) printf (" ");
		}
		printf("%02X ", (unsigned char) buffer[i]);
	}
	printf("\n");
}

template<size_t N>
void dump(char const (&buffer)[N])
{
	dump(buffer, N);
}

void dump(buffer const & buf)
{
	dump(buf.data, buf.capacity);
}


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

long to_int32(buffer const & buf)
{
	return (long) (unsigned long) (buf.data[0] << 24) | (buf.data[1] << 16) | (buf.data[2] << 8) | buf.data[3];
}

long to_int32_be(buffer const & buf)
{
	return (long) (unsigned long) (buf.data[0] << 24) | (buf.data[1] << 16) | (buf.data[2] << 8) | buf.data[3];
}

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

}

#endif // _IO_HPP_
