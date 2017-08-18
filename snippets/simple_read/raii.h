#ifndef _raii_h_
#define _raii_h_

#include <stdio.h>

namespace raii {

struct InputFile
{
	FILE * handle = nullptr;
	char * path = nullptr;

	InputFile(char const * path);
	virtual ~InputFile();

	inline operator FILE * () { return handle; }
};

}

#endif // _raii_h_
