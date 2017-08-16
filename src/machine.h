#ifndef _MACHINE_H_
#define _MACHINE_H_

#include <map>
#include <string>
#include <stdarg.h>

#define ECHO_OFF

void emit(char const *, ...);

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

	__mingw_fprintf(stdout, "* %s", buffer);
	return true;
#endif
	return false;
}

namespace machine {

struct Logger
{
	FILE * file = nullptr;
	int lineno = 0;
	static size_t const N = 1024;

	Logger(char const * path, bool force_overwrite=true, bool truncate=true);
	~Logger();

	void out(char const * format, ...) const;
	inline void inc() { ++lineno; }
};

struct Context
{
	enum { DT_NONE, DT_KEY, DT_STRING, DT_BYTES, DT_FIXED32, DT_INT32 } dtyp;
	unsigned long long val;

	// struct Assignop
	enum { AT_NONE, AT_IN, AT_II, AT_IR } at;
	enum { AO_NONE, AO_ASS, AO_ADD, AO_SUB } ao;

	// struct Readop
	enum { R_NONE, R_D, R_DFI, R_I, R_IFI } readop;
	char dst[255 + 1];
	char src[255 + 1];
	char rid[255 + 1];	// Used in readop1
	// int dtyp;
	enum { ST_LE, ST_BE } styp;
};

struct Var
{
	union {
		unsigned long long u;
		signed long long s;
	} val;
	unsigned char * buf = nullptr;

	~Var();

	Var & operator=(Var & var);
	Var & operator+=(Var & var);
	Var & operator-=(Var & var);

	// void assign(int op, unsigned long long uval);
	// void assign(int op, signed long long sval);
	// void assign(char * buf);
	void dump() const;
};

struct Vars
{
	std::map<std::string, Var> data;

	void assign(char const * key, int op, Var & acc);
	void assign(char const * key, int op, unsigned long long const & val);
	void assign(char const * key_dst, int op, char const * key_src);
	void dump() const;
};

struct Labels
{
	std::map<std::string, size_t> data;

	void set(char const * key, size_t line);
	void dump() const;
};

struct State
{
	Vars vars;
	Labels labels;
	machine::Var acc;

	char file_path[260 + 1] = {0};
	FILE * file = nullptr;

	~State();

	void open(char const * path);
	void dump() const;
	void assignment(Context const & context);
	void read(Context const & context);
	void yield_si(char const * str, char const * key);
};

void read_fixed32(FILE * file, int dtyp, int styp, machine::Var & acc);

void flip_32(unsigned long long * dst, unsigned long const * src);

}

#endif // _MACHINE_H_
