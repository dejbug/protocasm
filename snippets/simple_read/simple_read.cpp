#include <stdio.h>
#include <assert.h>

#include "common.hpp"
#include "pb.h"
#include "osm.h"

int main()
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
