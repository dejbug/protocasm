#include <assert.h>
#include <stdio.h>

#include <google/protobuf/io/zero_copy_stream_impl_lite.h>

#include "common.hpp"
#include "goo.h"

int test_1(char const *);
int test_2(char const *, bool respect_bh_len = false);

int main()
{
	char const * path = "..\\..\\data\\Darmstadt.osm.pbf";

	return test_2(path);
	// return test_1(path);
	return 0;
}

int test_2(char const * path, bool respect_bh_len)
{
	goo::context ctx(path);
	OSMPBF::BlobHeader bh;

	if (respect_bh_len)
	{
		auto bh_len = goo::read_bh_len(ctx);
		printf("- bh_len = %d\n", bh_len);
		bh = goo::read_bh(ctx, bh_len);
	}

	else
	{
		ctx.coded_input->Skip(4);
		bh = goo::read_bh(ctx);
	}

	printf("- bh.type = '%s'\n", bh.type().c_str());
	printf("- bh.datasize = %d\n", bh.datasize());

	return 0;
}

int test_1(char const * path)
{
	goo::context ctx(path);

	goo::uint32 bh_len = 0;
	ctx.coded_input->ReadLittleEndian32(&bh_len);
	bh_len = goo::flip(bh_len);
	fprintf(stderr, "- bh_len = %d\n", bh_len);

	assert(14 == bh_len);

	goo::uint32 key_1;
	ctx.coded_input->ReadVarint32(&key_1);
	fprintf(stderr, "- key_1 = %d\n", key_1);

	assert(0x0A == key_1);

	return 0;
}
