#ifndef _PB_H_
#define _PB_H_

#include <string>
#include "common.hpp"
#include "input.h"

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

namespace trans {

typ::i4 flip(typ::i4);
typ::i8 flip(typ::i8);

} // namespace trans

namespace io {

// size_t read(FILE * file, char * buffer, size_t size);
typ::string read_bytes(input::iinput &, size_t size);
typ::string read_string(input::iinput &);
typ::key read_key(input::iinput &);

typ::i4 read_i4(input::iinput &);
typ::i8 read_i8(input::iinput &);

typ::i4 read_v4(input::iinput &);
typ::i8 read_v8(input::iinput &);

} // namespace io

} // namespace pb

#endif // _PB_H_
