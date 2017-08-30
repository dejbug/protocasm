#include <assert.h>
#include <stdio.h>
#include <zlib.h>

#include "fileformat.pb.h"
#include "osmformat.pb.h"

#include "common.hpp"
#include "goo.h"

int main()
{
	size_t const debug_max_loops = 16;
	char const * path = "..\\..\\data\\Darmstadt.osm.pbf";

	printf("- zlibVersion = '%s'\n", zlibVersion());

	goo::context ctx(path);

	// ctx.coded_input->Skip(4);
	for (size_t i=0; i<debug_max_loops; ++i)
	{
		if (!goo::more(ctx)) break;

		goo::dump_ahead(ctx, 32);
		auto const bh_len = goo::read_bh_len(ctx);

		auto bh = goo::read_bh(ctx, bh_len);
		printf("- '%s'\n", bh.GetTypeName().c_str());

		// goo::skip_bb(ctx, bh.datasize());
		auto bb = goo::read_bb(ctx, bh.datasize());
		printf("- '%s'\n", bb.GetTypeName().c_str());

		printf("- [ %s", bh.type().c_str());
		if (bb.has_raw()) printf(" raw %d", bh.datasize());
		else if (bb.has_zlib_data())
		{
			printf(" zlib", bh.datasize());
			if (bb.has_raw_size()) printf(" %d", bb.raw_size());
		}
		else throw common::make_error("main : unexpected Blob data format: neither 'raw' nor 'zlib'.");
		puts(" ]");

		if (bh.type() == "OSMHeader")
		{
		}
		else if (bh.type() == "OSMData")
		{
		}
		else throw common::make_error("main : unexpected BlobHeader type '%s'.", bh.type());

		auto hb = goo::read_hb(bb);
		printf("- '%s'\n", hb.GetTypeName().c_str());

	}

	return 0;
}
