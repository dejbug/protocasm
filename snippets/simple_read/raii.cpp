#include "raii.h"
#include "operations.h"

#include <string.h>

raii::InputFile::InputFile(char const * path)
{
	handle = fopen(path, "rb");
	if (!handle)
		throw make_error("InputFile::ctor : file not found %s", path);
	this->path = strdup(path);
}

raii::InputFile::~InputFile()
{
	delete path;
	fclose(handle);
}
