#include "lib.h"
#include <assert.h>
#include <string.h>

lib::typ::bytes::bytes()
	: data(nullptr), capacity(0), good(0)
{
}

lib::typ::bytes::bytes(size_t capacity)
	: data(nullptr), capacity(0), good(0)
{
	if (!capacity) throw common::make_error("lib::typ::bytes::ctor : capacity must be >0 in this ctor");
	grow(capacity);
}

lib::typ::bytes::~bytes()
{
	free();
}

lib::typ::bytes::bytes(lib::typ::bytes && other)
{
	data = other.data;
	capacity = other.capacity;
	good = other.good;
	other.drop();
}

void lib::typ::bytes::grow(size_t size)
{
	if (!size) return;
	if (capacity >= size) return;
	free();
	data = new char[capacity = size];
}

void lib::typ::bytes::drop()
{
	data = nullptr;
	capacity = good = 0;
}

void lib::typ::bytes::free()
{
	if (data) delete[] data;
	drop();
}

void lib::typ::bytes::fill(FILE * file)
{
	if (!capacity) return;
	read(file, capacity);
}

void lib::typ::bytes::read(FILE * file, size_t size)
{
	good = 0;

	if (size > capacity)
		throw common::make_error("lib::typ::bytes::read : not enough room for reading %d bytes from file at %08X: only %d bytes allocated", size, (size_t) file, capacity);

	good = fread(data, 1, size, file);

	if (good < size && !feof(file))
		throw common::make_error("lib::typ::bytes::read : error while reading from file at %08X: only %d of %d bytes read", (size_t) file, good, size);
}

void lib::typ::bytes::read(FILE * file)
{
	grow(10);

	for (size_t i = 0; i < capacity; ++i)
	{
		char byte[1];
		lib::io::read(file, byte);
		data[i] = 0x7F & byte[0];
		if (byte[0] & 0x80) continue;
		good = i + 1;
		return;
	}

	throw common::make_error("lib::typ::bytes::read : end of buffer reached at byte %d before varint fully read", capacity);
}

size_t lib::io::read(FILE * file, char * buffer, size_t size)
{
	size_t const good = fread(buffer, sizeof(char), size, file);
	if (good < size && !feof(file))
		throw common::make_error("lib::io::read : error while reading from file at %08X: only %d of %d bytes read", (size_t) file, good, size);
	return good;
}

void lib::io::dump(char const * buffer, size_t size)
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

void lib::io::dump(lib::typ::bytes & bytes)
{
	lib::io::dump(bytes.data, bytes.good);
}

lib::io::file::file(char const * path)
{
	handle = fopen(path, "rb");
	if (!handle)
		throw common::make_error("lib::io::file::ctor : file not found %s", path);
	this->path = strdup(path);
}

lib::io::file::~file()
{
	delete path;
	fclose(handle);
}

lib::typ::bytes lib::io::read(FILE * file, size_t size)
{
	lib::typ::bytes out(size);
	out.read(file, size);
	return out;
}

lib::typ::bytes lib::io::read(FILE * file)
{
	lib::typ::bytes out(10);
	out.read(file);
	return out;
}

void lib::io::read(FILE * file, lib::typ::i4 & out)
{
	assert(sizeof(lib::typ::i4) == 4);
	lib::io::read(file, (char *) &out, 4);
}

void lib::io::read(FILE * file, lib::typ::i8 & out)
{
	assert(sizeof(lib::typ::i8) == 8);
	lib::io::read(file, (char *) &out, 8);
}

#define SHL32(a, i) (((lib::typ::i4) (((unsigned char const *) a)[i])) << (24-i*8))
// #define SHL32(a, i, s) ((((unsigned char const *) a)[i]) << (s))

lib::typ::i4 lib::trans::flip(lib::typ::i4 value)
{
	// TODO: ws2_32.dll's htonl() does this much more
	// efficiently, using only scratch registers .

	return SHL32(&value, 0) | SHL32(&value, 1) | SHL32(&value, 2) | SHL32(&value, 3);

	auto x = (unsigned char const *) &value;
	return (long) (unsigned long) (x[0] << 24) | (x[1] << 16) | (x[2] << 8) | x[3];
}

#define SHL64(a, i) (((lib::typ::u8) (((unsigned char const *) a)[i])) << (56-i*8))

lib::typ::i8 lib::trans::flip(lib::typ::i8 value)
{
	return SHL64(&value, 0) | SHL64(&value, 1) | SHL64(&value, 2) | SHL64(&value, 3) | SHL64(&value, 4) | SHL64(&value, 5) | SHL64(&value, 6) | SHL64(&value, 7);
}
