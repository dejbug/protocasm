#pragma once
#include <map>
#include <string>
#include <stdexcept>
#include "protocasm.tab.h"

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
void log(char const * format, ...)
{
	char buffer[N + 1] = {0};

	va_list args;
	va_start(args, format);
	__mingw_vsnprintf(buffer, N, format, args);
	va_end(args);

	__mingw_fprintf(stdout, "* %s", buffer);
}

namespace machine {

struct Vars
{
	std::map<std::string, long long> vars;

	void assign(char const * key, int op, long long val)
	{
		long long const old_val = vars[key];

		switch (op)
		{
			case '=': vars[key] = val; break;
			case O_ADDA: vars[key] += val; break;
			case O_SUBA: vars[key] -= val; break;
			// ...
		}

		log("var '%s' changed from %lld to %lld", key, old_val, vars[key]);
	}

	void assign(char const * key_dst, int op, char const * key_src)
	{
		assign(key_dst, op, vars[key_src]);
	}

	void dump()
	{
		for (auto it = vars.cbegin(); it != vars.cend(); ++it)
			__mingw_printf("\n[var] %s (%lld)", it->first.c_str(), it->second);
		__mingw_printf("\n");
	}
};

struct State
{
	Vars vars;
	FILE * file = nullptr;

	void open(char const * path)
	{
		if (file) fclose(file);
		file = fopen(path, "rb");
		if (!file) throw make_error("file not found: \"%s\"", path);
		log("opened file \"%s\" for input", path);
	}
};

}
