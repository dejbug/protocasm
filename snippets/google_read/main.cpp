#include <assert.h>
#include <stdio.h>

#include "fileformat.pb.h"
#include "osmformat.pb.h"

#include "common.hpp"
#include "goo.h"

int main()
{
	size_t const debug_max_loops = 16;
	char const * path = "..\\..\\data\\Darmstadt.osm.pbf";

	goo::context ctx(path);

	// ctx.coded_input->Skip(4);
	for (size_t i=0; i<debug_max_loops; ++i)
	{
		if (!goo::more(ctx)) break;

		goo::dump_ahead(ctx, 32);
		auto const bh_len = goo::read_bh_len(ctx);

		OSMPBF::BlobHeader bh = goo::read_bh(ctx, bh_len);

		printf("- bh.type = '%s'\n", bh.type().c_str());
		printf("- bh.datasize = %d\n", bh.datasize());
		goo::skip_bb(ctx, bh.datasize());
	}

	return 0;
}
