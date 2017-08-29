#ifndef _INPUT_H_
#define _INPUT_H_

#include <assert.h>
#include <string.h>
#include "common.hpp"

namespace input {

struct iinput
{
	virtual size_t skip(size_t size) = 0;
	virtual size_t read(char * buffer, size_t size) = 0;
	virtual size_t more() const = 0;
};

template<size_t N=16>
struct buffer
{
	char * const data = new char[N];
	size_t const capacity = N;
	size_t good = 0;

	virtual ~buffer()
	{
		good = 0;
		if (data) delete[] data;
	}

	void fill(char c=0)
	{
		memset(data, c, capacity);
	}

	size_t read(input::iinput & iin)
	{
		return good = iin.read(data, capacity);
	}

	void hexdump(size_t const br_at = 16, size_t const sp_at = 4)
	{
		common::hexdump(data, good, br_at, sp_at);
	}
};

struct fileinput : public iinput
{
	FILE * file = nullptr;
	size_t file_size = 0;

	fileinput(char const * path);
	virtual ~fileinput();

	size_t skip(size_t size);
	size_t read(char * buffer, size_t size);
	size_t more() const;
};

struct heapinput : public iinput
{
	char * data = nullptr;
	size_t data_size = 0;
	bool manage_mem = false;
	size_t offset = 0;

	heapinput();
	heapinput(size_t capacity);
	virtual ~heapinput();

	void attach(char * buffer, size_t size, bool manage=false);
	void clone(char * buffer, size_t size);

	size_t skip(size_t size);
	size_t read(char * out, size_t out_size);
	size_t more() const;
};

} // namespace input

#endif // _INPUT_H_
