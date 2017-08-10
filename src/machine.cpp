#include "machine.h"
#include "protocasm.tab.h"
#include <stdexcept>
#include <string.h>


void machine::Vars::assign(char const * key, int op, unsigned long long val)
{
	unsigned long long const old_val = data[key];

	switch (op)
	{
		case Context::AO_ASS: data[key] = val; break;
		case Context::AO_ADD: data[key] += val; break;
		case Context::AO_SUB: data[key] -= val; break;
		// ...
	}

	log("var '%s' changed from %llu to %llu", key, old_val, data[key]);
}

void machine::Vars::assign(char const * key_dst, int op, char const * key_src)
{
	assign(key_dst, op, data[key_src]);
}

void machine::Vars::dump()
{
	for (auto it = data.cbegin(); it != data.cend(); ++it)
		__mingw_printf("[var] %16llu : %s\n", it->second, it->first.c_str());
}



void machine::Labels::set(char const * key, size_t line)
{
	data[key] = line;
	log("label '%s' set at line #%d", key, line);
}

void machine::Labels::dump()
{
	for (auto it = data.cbegin(); it != data.cend(); ++it)
		__mingw_printf("[label] '%s' at line #%d\n", it->first.c_str(), it->second);
}



machine::State::~State()
{
	if (file) fclose(file);
}

void machine::State::open(char const * path)
{
	if (file) fclose(file);

	strncpy(file_path, path, sizeof(file_path)-1);
	file = fopen(file_path, "rb");

	if (!file) throw make_error("file not found: \"%s\"", file_path);
	log("opened file \"%s\" for input", file_path);
}

void machine::State::dump()
{
	log("dump");

	__mingw_printf("\n\n");
	__mingw_printf("[file] \"%s\" at %08x\n", file_path, (size_t) file);
	vars.dump();
	labels.dump();
}

void machine::State::assignment(Context const & context)
{
	switch (context.at)
	{
		default: break;

		case Context::AT_IN:
			vars.assign(context.dst, context.ao, context.val);
			break;

		case Context::AT_II:
			vars.assign(context.dst, context.ao, context.src);
			break;

		case Context::AT_IR:
			printf("%s = ", context.dst);
			read(context);
			vars.assign(context.dst, context.ao, acc);
			break;
	}
}

void machine::State::read(Context const & context)
{
	printf("READ");
	switch (context.dtyp)
	{
		default: break;
		case Context::DT_FIXED32: printf(" FIXED32"); break;
	}
	switch (context.styp)
	{
		case Context::ST_LE: break;
		case Context::ST_BE: printf(" AS BigEndian"); break;
	}

	// if (Readop::Rd != readop.typ)
	// 	throw make_error("not implemented yet: read operation");

	// unsigned long long * const out = *readop.dst ? &vars.data[readop.dst] : nullptr;

	// switch (readop.dtyp)
	// {
	// 	// case T_FIXED32: read_fixed32(dtyp, styp, out); break;
	// }
}

void machine::State::read_fixed32(int dtyp, int styp, unsigned long long * out)
{
	unsigned long s32 = 0;
	fread(&s32, 4, 1, file);
	if (!out) return;
	if (S_BE == styp) flip_32(out, &s32);
	else *out = (unsigned long long) s32;
}

// void read_fixed32(int dtyp, int styp, char const * key=nullptr)
// {
// 	unsigned long s32 = 0;
// 	fread(&s32, 4, 1, file);
// 	if (!key) return;
// 	if (S_BE == styp) flip_32(&vars.data[key], &s32);
// 	else vars.data[key] = (unsigned long long) s32;
// }

void machine::State::flip_32(unsigned long long * d, unsigned long * s)
{
	// TODO: ws2_32.dll's htonl() does this using only
	// scratch registers .
	d[0] = s[3];
	d[1] = s[2];
	d[2] = s[1];
	d[3] = s[0];
}

void machine::State::yield_si(char const * str, char const * key)
{
	log("yield_si '%s' %lld", str, vars.data[key]);
}
