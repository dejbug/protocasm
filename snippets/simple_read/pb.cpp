#include "pb.h"
#include "assert.h"

pb::typ::string pb::io::read_bytes(input::iinput & iin, size_t size)
{
	pb::typ::string str;
	str.resize(size);
	iin.read((char *) str.data(), size);
	return str;
}

pb::typ::string pb::io::read_string(input::iinput & iin)
{
	pb::typ::i8 const str_size = pb::io::read_v8(iin);
	// ECHO2(08llx, lld, str_size);

	pb::typ::string const str = pb::io::read_bytes(iin, str_size);
	// ECHO1(s, str.c_str());

	return str;
}

pb::typ::key pb::io::read_key(input::iinput & iin)
{
	pb::typ::u8 const key = pb::io::read_v8(iin);
	// ECHO2(08llx, lld, key);
	return {key >> 3, key & 0x7};
}

pb::typ::i4 pb::io::read_i4(input::iinput & iin)
{
	pb::typ::i4 out = 0;
	assert(sizeof(out) == 4);
	iin.read((char *) &out, sizeof(out));
	return out;
}

pb::typ::i8 pb::io::read_i8(input::iinput & iin)
{
	pb::typ::i8 out = 0;
	assert(sizeof(out) == 8);
	iin.read((char *) &out, sizeof(out));
	return out;
}

#define SHL4(a, i) (((pb::typ::i4) (((unsigned char const *) a)[i])) << (24-i*8))

pb::typ::i4 pb::trans::flip(typ::i4 value)
{
	// TODO: ws2_32.dll's htonl() does this much more
	// efficiently, using only scratch registers .

	return SHL4(&value, 0) | SHL4(&value, 1) | SHL4(&value, 2) | SHL4(&value, 3);
}

#define SHL8(a, i) (((pb::typ::u8) (((unsigned char const *) a)[i])) << (56-i*8))

pb::typ::i8 pb::trans::flip(typ::i8 value)
{
	return SHL8(&value, 0) | SHL8(&value, 1) | SHL8(&value, 2) | SHL8(&value, 3) | SHL8(&value, 4) | SHL8(&value, 5) | SHL8(&value, 6) | SHL8(&value, 7);
}

template<class T>
T read_v(input::iinput & iin)
{
	T out = 0;

	size_t i = 0;
	for (; i < 10; ++i)
	{
		char byte;
		if (0 == iin.read(&byte, sizeof(char))) break;

		out |= (0x7F & byte) << (7*i);

		if (0 == (byte & 0x80)) break;
	}

	return out;
}

pb::typ::i4 pb::io::read_v4(input::iinput & iin)
{
	return read_v<typ::i4>(iin);
}

pb::typ::i8 pb::io::read_v8(input::iinput & iin)
{
	return read_v<typ::i8>(iin);
}
