#pragma once
#include <map>
#include <string>
#include "protocasm.tab.h"

extern std::map<std::string, long long> vars;

namespace machine {

struct Var
{
	enum {
		t_null = -1,
		t_le,
		t_be,
	};
	int t = t_null;
	union {
		long long s;
		unsigned long long u;
		float f;
		double d;
	};
	std::string b;
};

void assign(char const * key, int op, long long val)
{
	switch (op)
	{
		case '=': vars[key] = val; break;
		case O_ADDA: vars[key] += val; break;
		case O_SUBA: vars[key] -= val; break;
		// ...
	}
}

void assign(char const * key, int op, char const * val)
{
	assign(key, op, vars[val]);
}

void dump()
{
	for (auto it = vars.cbegin(); it != vars.cend(); ++it)
		printf("\n[var] %s (%ld)", it->first.c_str(), it->second);
	printf("\n");
}

}
