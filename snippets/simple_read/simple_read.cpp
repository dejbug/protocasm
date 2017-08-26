#include <stdio.h>
#include <string.h>

#include "common.hpp"
#include "pb.h"
#include "osm.h"

int main()
{
	size_t const debug_max_loops = 8;

	char const * path = "..\\..\\data\\Darmstadt.osm.pbf";
	pb::raii::file file(path);

	pb::typ::i4 const osm_bh_size = pb::trans::flip(pb::io::read_i4(file));
	ECHO2(08lx, ld, osm_bh_size);

	bool more = true;
	for (size_t i=0; more && i<debug_max_loops; ++i)
	{
		pb::typ::i8 const key = pb::io::read_v<pb::typ::i8>(file);
		ECHO2(08llx, lld, key);

		pb::typ::i8 const id = key >> 3;
		ECHO2(08llx, lld, id);
		pb::typ::u1 const wt = key & 0x7;
		ECHO2(08x, d, wt);

		switch (wt)
		{
			default: more = false; break;

			case 2:
			{
				pb::typ::string const str = pb::io::read_string(file);
				ECHO1(s, str.c_str());
				break;
			}
		}
	}

	return 0;
}
