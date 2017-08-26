#include "pb.h"
#include "assert.h"

pb::raii::file::file(char const * path)
{
	handle = fopen(path, "rb");
	if (!handle) throw common::make_error("lib::io::file::ctor : file not found %s", path);
}

pb::raii::file::~file()
{
	if (handle) fclose(handle);
}

pb::typ::string pb::io::read_bytes(FILE * file, size_t size)
{
	pb::typ::string str;
	str.resize(size);
	common::read(file, (char *) str.data(), size);
	return str;
}

pb::typ::string pb::io::read_string(FILE * file)
{
	pb::typ::i8 const str_size = pb::io::read_v8(file);
	// ECHO2(08llx, lld, str_size);

	pb::typ::string const str = pb::io::read_bytes(file, str_size);
	// ECHO1(s, str.c_str());

	return str;
}

pb::typ::key pb::io::read_key(FILE * file)
{
	pb::typ::u8 const key = pb::io::read_v8(file);
	// ECHO2(08llx, lld, key);
	return {key >> 3, key & 0x7};
}

pb::typ::i4 pb::io::read_i4(FILE * file)
{
	pb::typ::i4 out = 0;
	assert(sizeof(out) == 4);
	common::read(file, (char *) &out, sizeof(out));
	return out;
}

pb::typ::i8 pb::io::read_i8(FILE * file)
{
	pb::typ::i8 out = 0;
	assert(sizeof(out) == 8);
	common::read(file, (char *) &out, sizeof(out));
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
