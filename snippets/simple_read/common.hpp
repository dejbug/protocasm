#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>
#include <stdarg.h>
#include <stdexcept>

// #define COMMON_ECHO_OFF

#define ECHO1(t, expr) __mingw_printf(#expr " : %" #t "\n", (expr))
#define ECHO2(t1, t2, expr) { auto const v = (expr); __mingw_printf(#expr " : %" #t1 " (%" #t2 ")\n", v, v); }

namespace common {

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

template<size_t N=1024>
bool echo(char const * format, ...)
{
#ifndef COMMON_ECHO_OFF
	char buffer[N + 1] = {0};

	va_list args;
	va_start(args, format);
	__mingw_vsnprintf(buffer, N, format, args);
	va_end(args);

	__mingw_fprintf(stderr, "* { %s }\n", buffer);
	return true;
#endif
	return false;
}

inline size_t read(FILE * file, char * buffer, size_t size)
{
	if (!size) return 0;

	long const mark = ftell(file);

	size_t const good = fread(buffer, sizeof(char), size, file);

	if (good < size && !feof(file))
		throw common::make_error("common::read : error while reading byte %ld from file %08X: need %d more bytes (got only %d)", mark + good, (size_t) file, size-good, good);

	return good;
}

template<size_t N>
size_t read(FILE * file, char (&buffer)[N])
{
	return read(file, buffer, N);
}

}

#endif // _COMMON_H_
