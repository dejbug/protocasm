#ifndef _raii_h_
#define _raii_h_

#include <stdio.h>
#include "common.hpp"
#include "operations.h"

namespace raii {

struct ifile
{
	FILE * handle = nullptr;
	char * path = nullptr;
	long mark = 0;

	ifile(char const * path);
	virtual ~ifile();

	inline operator FILE * () { return handle; }

};


struct buffer : public common::typ::bytes
{
	buffer(size_t capacity);
	virtual ~buffer();

	void read(FILE * file, size_t size);
	void read(FILE * file);
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
