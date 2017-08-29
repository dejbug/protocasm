#include "input.h"

input::fileinput::fileinput(char const * path)
{
	file = fopen(path, "rb");
	if (!file) throw common::make_error("fileinput::ctor : file not found %s", path);

	fseek(file, 0, SEEK_END);
	file_size = ftell(file);
	fseek(file, 0, SEEK_SET);
}

input::fileinput::~fileinput()
{
	if (file) fclose(file);
	file = nullptr;
	file_size = 0;
}

size_t input::fileinput::read(char * buffer, size_t size)
{
	if (!size) return 0;

	long const mark = ftell(file);

	size_t const good = fread(buffer, sizeof(char), size, file);

	if (good == size) return good;

	if(feof(file)) throw common::make_error<common::err::eof>("fileinput::read : EOF while reading byte %lu (%08X) from file %08X: need %u more bytes (got only %u)", mark + good, mark + good, (size_t) file, size-good, good);

	throw common::make_error("fileinput::read : unknown error while reading byte %lu (%08X) from file %08X: need %u more bytes (got only %u)", mark + good, mark + good, (size_t) file, size-good, good);
}

size_t input::fileinput::more() const
{
	size_t const mark = ftell(file);
	return mark < file_size ? file_size - mark : 0;
}

input::heapinput::~heapinput()
{
	if (manage_mem && data) delete[] data;
	data = nullptr;
	data_size = 0;
}

void input::heapinput::attach(char * buffer, size_t size)
{
	data = buffer;
	data_size = size;
	manage_mem = false;
	offset = 0;
}

void input::heapinput::clone(char * buffer, size_t size)
{
	assert(size > 0);
	data_size = size;
	data = new char[data_size];
	memcpy(data, buffer, data_size);
	manage_mem = true;
	offset = 0;
}

size_t input::heapinput::read(char * out, size_t out_size)
{
	// todo: test!

	if (offset >= data_size) throw common::make_error<common::err::eof>("heapinput::read : end of data");

	size_t const data_left_size = data_size - offset;
	size_t const copy_size = MIN(out_size, data_left_size);
	memcpy(out, data + offset, copy_size);
	offset += copy_size;

	return copy_size;
}

size_t input::heapinput::more() const
{
	return offset < data_size;
}
