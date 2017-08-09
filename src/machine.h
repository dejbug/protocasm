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
	std::map<std::string, long long> data;

	void assign(char const * key, int op, long long val)
	{
		long long const old_val = data[key];

		switch (op)
		{
			case '=': data[key] = val; break;
			case O_ADDA: data[key] += val; break;
			case O_SUBA: data[key] -= val; break;
			// ...
		}

		log("var '%s' changed from %lld to %lld", key, old_val, data[key]);
	}

	void assign(char const * key_dst, int op, char const * key_src)
	{
		assign(key_dst, op, data[key_src]);
	}

	void dump()
	{
		for (auto it = data.cbegin(); it != data.cend(); ++it)
			__mingw_printf("[var] %s (%lld)\n", it->first.c_str(), it->second);
	}
};

struct Labels
{
	std::map<std::string, size_t> data;

	void set(char const * key, size_t line)
	{
		data[key] = line;
		log("label '%s' set at line #%d", key, line);
	}

	void dump()
	{
		for (auto it = data.cbegin(); it != data.cend(); ++it)
			__mingw_printf("[label] %s (%d)\n", it->first.c_str(), it->second);
	}
};

struct State
{
	Vars vars;
	Labels labels;
	FILE * file = nullptr;

	void open(char const * path)
	{
		if (file) fclose(file);
		file = fopen(path, "rb");
		if (!file) throw make_error("file not found: \"%s\"", path);
		log("opened file \"%s\" for input", path);
	}

	void dump()
	{
		__mingw_printf("\n");
		__mingw_printf("[file] %08x\n", (size_t) file);
		vars.dump();
		labels.dump();
	}

	void yield_si(char const * str, char const * key)
	{
		log("yield_si '%s' %lld", str, vars.data[key]);
	}

};

}
