#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "common.hpp"
#include "pb.h"
#include "osm.h"
#include "input.h"

int test();

int main()
{
	char const * path = "..\\..\\data\\Darmstadt.osm.pbf";

	input::buffer<64> buf;

	input::fileinput fin(path);
	buf.read(fin);
	fputs("---\n", stdout);
	buf.hexdump();

	input::heapinput hin;
	hin.clone(buf.data, buf.capacity);
	fputs("---\n", stdout);
	common::hexdump(hin.data, hin.data_size);

	buf.fill(0);
	fputs("---\n", stdout);
	buf.hexdump();

	buf.read(hin);
	fputs("---\n", stdout);
	buf.hexdump();

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
