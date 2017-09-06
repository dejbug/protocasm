#include <assert.h>
#include <stdio.h>
#include <zlib.h>

#include "fileformat.pb.h"
#include "osmformat.pb.h"

#include "common.hpp"
#include "goo.h"

int main(int argc, char ** argv)
{
	if (argc < 2)
	{
		fprintf(stderr, "usage: %s PATH\n", argv[0]);
		return -1;
	}

	char const * const path = argv[1];

	try
	{
		// goo::info_parser parser(path);
		// goo::unzip_parser parser(path);
		goo::waycount_parser parser(path);
		parser.run();
	}
	catch (std::runtime_error & e)
	{
		printf("[ ERROR ] %s\n", e.what());
		return -1;
	}

	return 0;
}
