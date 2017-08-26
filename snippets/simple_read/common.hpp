#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>
#include <stdarg.h>
#include <stdexcept>

// #define COMMON_ECHO_OFF

#define ECHO(t, expr) __mingw_printf("+ %" #t "\n", (expr))
#define ECHO1(t, expr) __mingw_printf("- " #expr " : %" #t "\n", (expr))
#define ECHO2(t1, t2, expr) { auto const v = (expr); __mingw_printf("- " #expr " : %" #t1 " (%" #t2 ")\n", v, v); }

#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))

#define DEFERR(name) struct name : public std::runtime_error { using std::runtime_error::runtime_error; }

namespace common {

namespace err {

DEFERR(eof);

} // namespace err

size_t const MAX_BUF_LEN = 1024;

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

inline size_t filesize(FILE * file)
{
	size_t const mark = ftell(file);
	fseek(file, 0, SEEK_END);
	size_t const size = ftell(file);
	fseek(file, mark, SEEK_SET);
	return size;
}

inline size_t read(FILE * file, char * buffer, size_t size)
{
	if (!size) return 0;

	long const mark = ftell(file);

	size_t const good = fread(buffer, sizeof(char), size, file);

	if (good < size)
	{
		if(feof(file)) throw common::make_error<common::err::eof>("common::read : EOF while reading byte %lu (%08X) from file %08X: need %u more bytes (got only %u)", mark + good, mark + good, (size_t) file, size-good, good);

		else throw common::make_error("common::read : unknown error while reading byte %lu (%08X) from file %08X: need %u more bytes (got only %u)", mark + good, mark + good, (size_t) file, size-good, good);
	}

	return good;
}

template<size_t N>
size_t read(FILE * file, char (&buffer)[N])
{
	return read(file, buffer, N);
}

inline void hexdump(char const * buffer, size_t size)
{
	size_t const br_at = 24;
	size_t const sp_at = 4;

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

}

#endif // _COMMON_H_
