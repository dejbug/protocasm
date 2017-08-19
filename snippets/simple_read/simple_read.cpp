#include <stdio.h>
#include <string.h>

#include "operations.h"
#include "raii.h"
// #include "snippets.h"


template<size_t N>
struct Buffer
{
	char * data = new char[N];
	size_t const size = N;
	size_t good = 0;
	size_t done = 0;

	virtual ~Buffer()
	{
		delete data;
	}

	void read(FILE * file)
	{
		good = op::read(file, data, size);
	}

	bool more() const
	{
		return done < good;
	}
};

struct Reader
{
	FILE * file = nullptr;

	Reader(FILE * file)
		: file(file)
	{
	}

	virtual ~Reader()
	{
	}

	void read(char * buffer, size_t bufsize)
	{
	}
};


int main()
{
	char const * path = "..\\..\\data\\Darmstadt.osm.pbf";

	/// OPEN "..."
	raii::InputFile file(path);

	op::read_fixed32(file);

	op::Varint varint;
	varint.read(file);
	printf("int from varint : %ld\n", varint.to_int32());
	varint.read(file);

	return 0;

	/// READ 4
	/// FLIP 4
	unsigned int const blobheader_size = op::flip_32(op::read_fixed32(file));
	printf("blobheader_size = %d\n", blobheader_size);

	/// SET HARD LIMIT AT ACC
	char * const blobheader_data = new char[blobheader_size];
	op::read(file, blobheader_data, blobheader_size);

	printf("blobheader_data = \n");
	op::dump(blobheader_data, blobheader_size);
	printf("\n");

	return 0;
}
