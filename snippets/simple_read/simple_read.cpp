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

	/// blobheader_size = READ FIXED32 AS BigEndian
	unsigned int const blobheader_size = op::flip_32(op::read_fixed32(file));
	printf("blobheader_size = %d\n", blobheader_size);

	/// We must only read the next {blobheader_size} bytes, so we
	/// set a mark at the current file pos and check against it after
	/// every read operation to see whether we have exceeded the
	/// blobheader's advertised size . If so, this would indicate an
	/// unexpected error .

	/// mark = MARK
	op::Mark mark(file);
	mark.set();

	while (mark < blobheader_size)
	{
		/// varint = READ VARINT
		op::Varint varint;
		varint.read(file);

		/// key = varint AS KEY
		op::Key key = varint.to_key();
		/// DUMP key
		printf("key id %d wire type %d\n", key.id, key.wt);

		switch (key.wt)
		{
			case 3:
			{
				std::string s = op::read_string(file);
				printf("string |%s|\n", s.c_str());
				break;
			}
		}
	}

	/// FAIL IF MARK - mark >= blobheader_size
	if (mark >= blobheader_size)
		throw make_error("read past hard limit: either our varint-reading logic has a bug, or the input file's generating application is at fault");

	return 0;
}
