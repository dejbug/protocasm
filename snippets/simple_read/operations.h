#ifndef _OPERATIONS_H_
#define _OPERATIONS_H_

#include <stdio.h>
#include <stdarg.h>
#include <stdexcept>

template<size_t N=1024, class E=std::runtime_error>
E make_error(char const * format, ...)
{
	char buffer[N + 1] = {0};

	va_list args;
	va_start(args, format);
	__mingw_vsnprintf(buffer, N, format, args);
	va_end(args);

	return E(buffer);
}

namespace op {

size_t read(FILE * file, char * buffer, size_t size);
unsigned int flip_32(unsigned int value);
unsigned int read_fixed32(FILE * file);
void dump(char const * buffer, size_t size);

} // namespace op

#endif // _OPERATIONS_H_
