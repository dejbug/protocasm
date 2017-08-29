#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "common.hpp"
#include "pb.h"
#include "osm.h"

struct input
{
	virtual size_t read(char * buffer, size_t size) = 0;
	virtual size_t more() const = 0;
};

struct fileinput : public input
{
	FILE * file = nullptr;
	size_t file_size = 0;

	fileinput(char const * path)
	{
		file = fopen(path, "rb");
		if (!file) throw common::make_error("fileinput::ctor : file not found %s", path);

		fseek(file, 0, SEEK_END);
		file_size = ftell(file);
		fseek(file, 0, SEEK_SET);
	}

	virtual ~fileinput()
	{
		if (file) fclose(file);
		file = nullptr;
		file_size = 0;
	}

	size_t read(char * buffer, size_t size)
	{
		if (!size) return 0;

		long const mark = ftell(file);

		size_t const good = fread(buffer, sizeof(char), size, file);

		if (good == size) return good;

		if(feof(file)) throw common::make_error<common::err::eof>("fileinput::read : EOF while reading byte %lu (%08X) from file %08X: need %u more bytes (got only %u)", mark + good, mark + good, (size_t) file, size-good, good);

		throw common::make_error("fileinput::read : unknown error while reading byte %lu (%08X) from file %08X: need %u more bytes (got only %u)", mark + good, mark + good, (size_t) file, size-good, good);
	}

	size_t more() const
	{
		size_t const mark = ftell(file);
		return mark < file_size ? file_size - mark : 0;
	}
};

struct heapinput : public input
{
	char * data = nullptr;
	size_t data_size = 0;
	bool manage_mem = false;
	size_t offset = 0;

	virtual ~heapinput()
	{
		if (manage_mem && data) delete[] data;
		data = nullptr;
		data_size = 0;
	}

	void attach(char * buffer, size_t size)
	{
		data = buffer;
		data_size = size;
		manage_mem = false;
		offset = 0;
	}

	void clone(char * buffer, size_t size)
	{
		assert(size > 0);
		data_size = size;
		data = new char[data_size];
		memcpy(data, buffer, data_size);
		manage_mem = true;
		offset = 0;
	}

	size_t read(char * out, size_t out_size)
	{
		// todo: test!

		if (offset >= data_size) throw common::make_error<common::err::eof>("heapinput::read : end of data");

		size_t const data_left_size = data_size - offset;
		size_t const copy_size = MIN(out_size, data_left_size);
		memcpy(out, data + offset, copy_size);
		offset += copy_size;
		return 0;
	}

	size_t more() const
	{
		return offset < data_size;
	}
};

int main()
{
	char const * path = "..\\..\\data\\Darmstadt.osm.pbf";

	char buffer[64] = {0};

	fileinput fin(path);
	fin.read(buffer, sizeof(buffer));
	fputs("\n", stdout);
	common::hexdump(buffer, sizeof(buffer));

	heapinput hin;
	hin.clone(buffer, sizeof(buffer));
	fputs("\n", stdout);
	common::hexdump(hin.data, hin.data_size);

	memset(buffer, 0, sizeof(buffer));
	fputs("\n", stdout);
	common::hexdump(buffer, sizeof(buffer));

	hin.read(buffer, sizeof(buffer));
	fputs("\n", stdout);
	common::hexdump(buffer, sizeof(buffer));

	return 0;
}

int test()
{
	size_t const debug_max_loops = 16;

	char const * path = "..\\..\\data\\Darmstadt.osm.pbf";
	pb::raii::file file(path);

	for (size_t i=0; i<debug_max_loops; ++i)
	{
		try {
			auto bh = osm::io::read_bh(file);
			osm::dbg::dump(bh);

			auto bb = osm::io::read_bb(file, bh.datasize);
			osm::dbg::dump(bb);
		}

		catch (osm::err::end &)
		{
			break;
		}
	}

	return 0;
}
