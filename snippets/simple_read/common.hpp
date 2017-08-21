#ifndef _COMMON_H_
#define _COMMON_H_

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

#endif // _COMMON_H_
