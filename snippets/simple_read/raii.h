#ifndef _raii_h_
#define _raii_h_

#include <stdio.h>
#include "operations.h"

namespace raii {

struct InputFile
{
	FILE * handle = nullptr;
	char * path = nullptr;
	long mark = 0;

	InputFile(char const * path);
	virtual ~InputFile();

	inline operator FILE * () { return handle; }

};


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

}

#endif // _raii_h_
