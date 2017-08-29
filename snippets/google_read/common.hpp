#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>
#include <stdarg.h>
#include <stdexcept>

#define MAX_BUF_LEN 1024

#define ECHO(t, expr) __mingw_printf("+ %" #t "\n", (expr))
#define ECHO1(t, expr) __mingw_printf("- " #expr " : %" #t "\n", (expr))
#define ECHO2(t1, t2, expr) { auto const v = (expr); __mingw_printf("- " #expr " : %" #t1 " (%" #t2 ")\n", v, v); }

#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))

#define DEFERR(name) struct name : public std::runtime_error { using std::runtime_error::runtime_error; }

namespace common {

template<class E=std::runtime_error, size_t N=MAX_BUF_LEN>
E make_error(char const * format, ...)
{
	char buffer[N + 1] = {0};

	va_list args;
	va_start(args, format);
	__mingw_vsnprintf(buffer, N, format, args);
	va_end(args);

	return E(buffer);
}

template<class T>
void hexdump(T const * data, size_t size, size_t const br_at = 16, size_t const sp_at = 4)
{
	auto buffer = (char const *) data;
	for (size_t i=0; i<size; ++i)
	{
		if (i > 0)
		{
			if(i % br_at == 0) printf("\n");
			else if(i % sp_at == 0) printf (" ");
		}
		printf("%02X ", (unsigned char) buffer[i]);
	}
	printf("\n");
}

inline size_t filesize(FILE * file)
{
	size_t const mark = ftell(file);
	fseek(file, 0, SEEK_END);
	size_t const size = ftell(file);
	fseek(file, mark, SEEK_SET);
	return size;
}

} // namespace common

#endif // _COMMON_H_
