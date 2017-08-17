#include "operations.h"

size_t op::read(FILE * file, char * buffer, size_t size)
{
	size_t const good = fread(buffer, 1, size, file);
	if (good < size && !feof(file))
		throw make_error("File::read : error while reading from file at %08X: only %d of %d bytes read", (size_t) file, good, size);
	return good;
}

unsigned int op::flip_32(unsigned int value)
{
	// TODO: ws2_32.dll's htonl() does this much more
	// efficiently, using only scratch registers .

	auto x = (unsigned char const *) &value;
	return (unsigned int) (x[0] << 24) | (x[1] << 16) | (x[2] << 8) | x[3];
}

unsigned int op::read_fixed32(FILE * file)
{
	unsigned int value = 0;
	read(file, (char *) &value, 4);
	return value;
}

void op::dump(char const * buffer, size_t size)
{
	for (size_t i=0; i<size; ++i)
	{
		if (i > 0)
		{
			if(i % 16 == 0) printf("\n");
			else if(i % 4 == 0) printf (" ");
		}
		printf("%02X ", (unsigned char) buffer[i]);
	}
	printf("\n");
}

