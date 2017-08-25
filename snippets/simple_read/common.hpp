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

template<size_t N=1024>
bool echo(char const * format, ...)
{
#ifndef ECHO_OFF
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

}

#endif // _COMMON_H_
