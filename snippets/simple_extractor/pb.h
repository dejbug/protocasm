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
// typedef std::string bytes;

struct key
{
	unsigned long long id : 61;
	unsigned long long wt : 3;
};

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

// size_t read(FILE * file, char * buffer, size_t size);
typ::string read_bytes(FILE * file, size_t size);
typ::string read_string(FILE * file);
typ::key read_key(FILE * file);

typ::i4 read_i4(FILE * file);
typ::i8 read_i8(FILE * file);

typ::i4 read_v4(FILE * file);
typ::i8 read_v8(FILE * file);

} // namespace io

} // namespace pb

#endif // _PB_H_
