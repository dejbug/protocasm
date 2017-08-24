#include "raii.h"
#include "operations.h"

#include <string.h>

raii::ifile::ifile(char const * path)
{
	handle = fopen(path, "rb");
	if (!handle)
		throw common::make_error("ifile::ctor : file not found %s", path);
	this->path = strdup(path);
}

raii::ifile::~ifile()
{
	delete path;
	fclose(handle);
}

raii::buffer::buffer(size_t capacity)
	: capacity(capacity)
{
	if (capacity > 0)
		data = new char[capacity];
}

raii::buffer::~buffer()
{
	capacity = 0;
	if (data)
	{
		delete[] data;
		data = nullptr;
	}
}

void raii::buffer::read(FILE * file, size_t size)
{
	good = 0;

	if (size > capacity)
		throw common::make_error("buffer::read : not enough room for reading %d bytes from file at %08X: only %d bytes allocated", size, (size_t) file, capacity);

	good = fread(data, 1, size, file);

	if (good < size && !feof(file))
		throw common::make_error("buffer::read : error while reading from file at %08X: only %d of %d bytes read", (size_t) file, good, size);
}

void raii::buffer::read(FILE * file)
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
