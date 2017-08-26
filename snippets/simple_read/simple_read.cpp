#include <stdio.h>
#include <string.h>

#include "common.hpp"
#include "pb.h"
#include "osm.h"

int main()
{
	size_t const debug_max_loops = 16;

	char const * path = "..\\..\\data\\Darmstadt.osm.pbf";
	pb::raii::file file(path);

	pb::typ::i4 const osm_bh_size = pb::trans::flip(pb::io::read_i4(file));
	ECHO2(08lx, ld, osm_bh_size);

	bool more = true;
	for (size_t i=0; more; ++i)
	{
		printf("-----------------------------------------------  loop %d", i);
		if (debug_max_loops) printf(" (of %d {debug})", debug_max_loops);
		printf("\n");

		if (debug_max_loops && i >= debug_max_loops) break;

		pb::typ::key const key = pb::io::read_key(file);
		ECHO2(08llx, lld, key.id);
		ECHO2(08llx, lld, key.wt);

		switch (key.wt)
		{
			default: more = false; break;

			case 0:
			{
				pb::typ::u8 const vi = pb::io::read_v8(file);
				ECHO2(08llx, lld, vi);
				break;
			}

			case 2:
			{
				pb::typ::string const str = pb::io::read_string(file);

				if (1 == key.id) ECHO1(s, str.c_str());
				else common::hexdump(str.c_str(), str.size() >= 48 ? 48 : str.size());

				break;
			}
		}
	}

	return 0;
}
