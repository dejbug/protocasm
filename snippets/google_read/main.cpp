#include <assert.h>
#include <stdio.h>
#include <zlib.h>

#include "fileformat.pb.h"
#include "osmformat.pb.h"

#include "common.hpp"
#include "goo.h"

void run(char const * path);

int main(int argc, char ** argv)
{
	if (argc < 2)
	{
		fprintf(stderr, "usage: %s PATH\n", argv[0]);
		return -1;
	}

	char const * const path = argv[1];

	try { run(path); }
	catch (std::runtime_error & e)
	{
		printf("[ ERROR ] %s\n", e.what());
		return -1;
	}

	return 0;
}

void run(char const * path)
{
	size_t const debug_max_loops = 16;

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

		auto bb_desc = goo::describe_bb(bh, bb);
		printf("%s\n", bb_desc.c_str());

		if (bh.type() == "OSMHeader")
		{
			goo::inflate_zlib(bb);
			auto hb = goo::read_hb(bb);
			printf("- '%s'\n", hb.GetTypeName().c_str());
		}
		else if (bh.type() == "OSMData")
		{
			goo::inflate_zlib(bb);
			auto pb = goo::read_pb(bb);
			printf("- '%s'\n", pb.GetTypeName().c_str());

			auto pb_desc = goo::describe_pb(pb);
			printf("%s\n", pb_desc.c_str());
		}
		else throw common::make_error("main : unexpected BlobHeader type '%s': this exception should be skippepd instead of being thrown.", bh.type());

	}
}
