#ifndef _PB_H_
#define _PB_H_

#include "common.hpp"
#include <string>

namespace pb {

namespace typ {

typedef char i1;
typedef long i4;
typedef long s4;
typedef long long i8;
typedef long long s8;

typedef unsigned char u1;
typedef unsigned long u4;
typedef unsigned long long u8;

typedef std::string string;

} // namespace typ

namespace raii {

struct file
{
	FILE * handle = nullptr;

	file(char const * path);
	virtual ~file();

	operator FILE * () { return handle; }
};

} // namespace raii

namespace trans {

typ::i4 flip(typ::i4);
typ::i8 flip(typ::i8);

} // namespace trans

namespace io {

size_t read(FILE * file, char * buffer, size_t size);
typ::string read(FILE * file, size_t size);

typ::i4 read_i4(FILE * file);
typ::i8 read_i8(FILE * file);

template<class T>
T read_v(FILE * file)
{
	T out = 0;

	long const mark = ftell(file);

	size_t i = 0;
	for (; i < 10; ++i)
	{
		char byte;
		if (0 == fread(&byte, sizeof(char), 1, file)) break;

		out |= (0x7F & byte) << (7*i);

		if (0 == (byte & 0x80)) break;
	}

	if (feof(file)) throw common::make_error("pb::io::read_v4 : end of file %08x reached at", (size_t) file);

	if (ferror(file)) throw common::make_error("pb::io::read_v4 : unknown error %08x (%d) while reading byte %d of varint from file %08x at %ld", ferror(file), ferror(file), i, (size_t) file, mark);

	return out;
}

inline typ::i4 read_v4(FILE * file) { return read_v<typ::i4>(file);  }
inline typ::i8 read_v8(FILE * file) { return read_v<typ::i8>(file);  }

} // namespace io

} // namespace pb

#endif // _PB_H_
