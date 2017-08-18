#include <stdio.h>
#include <string.h>

#include "operations.h"
#include "raii.h"
// #include "snippets.h"


int main()
{
	char const * path = "..\\..\\data\\Darmstadt.osm.pbf";

	raii::InputFile file(path);

	unsigned int const blobheader_size = op::flip_32(op::read_fixed32(file));
	printf("blobheader_size = %d\n", blobheader_size);

	char * const blobheader_data = new char[blobheader_size];
	op::read(file, blobheader_data, blobheader_size);

	printf("blobheader_data = \n");
	op::dump(blobheader_data, blobheader_size);
	printf("\n");

	return 0;
}
