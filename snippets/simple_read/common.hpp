#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>
#include <stdarg.h>
#include <stdexcept>

#define ECHO(t, expr) __mingw_printf(#expr " : %" #t "\n", (expr))
#define ECHO2(t1, t2, expr) { auto const v = (expr); __mingw_printf(#expr " : %" #t1 " , %" #t2 "\n", v, v); }

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

template<size_t N, class T=char>
size_t read(FILE * file, T (&buffer)[N])
{
	size_t const good = fread(buffer, sizeof(T), N, file);
	if (good < N && !feof(file))
		throw make_error("common::read : error while reading from file at %08X: only %d of %d bytes read", (size_t) file, good, N);
	return good;
}

template<class T=char>
size_t read(FILE * file, T * buffer, size_t size)
{
	size_t const good = fread(buffer, 1, size, file);
	if (good < size && !feof(file))
		throw common::make_error("File::read : error while reading from file at %08X: only %d of %d bytes read", (size_t) file, good, size);
	return good;
}

}

#endif // _COMMON_H_
