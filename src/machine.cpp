#include "machine.h"
#include "protocasm.tab.h"
#include <stdexcept>
#include <string.h>
#include <time.h>
// #include "dbgout.h"



bool file_exists(char const * path)
{
	FILE * file = fopen(path, "rb");
	if (!file) return false;
	fclose(file);
	return true;
}

void fwrite_time(FILE * file, bool date=true, bool msec=true)
{
	char tmpbuf[32];

	if (date)
	{
		time_t now;
		time(&now);
		struct tm * const today = localtime(&now);

		strftime(tmpbuf, sizeof(tmpbuf), "%Y-%m-%d | %H:%M:%S", today);
		fputs(tmpbuf, file);
	}
	else
	{
		_strtime(tmpbuf);
		fputs(tmpbuf, file);
	}

	if (msec)
	{
		struct _timeb tstruct;
		_ftime(&tstruct);
		fprintf(file, ":%03d", tstruct.millitm);
	}
}

machine::Logger::Logger(char const * path, bool force_overwrite, bool truncate)
{
	if (!path || !*path)
		throw make_error("machine::Logger - invalid argument");

	if (!force_overwrite && file_exists(path))
		throw make_error("machine::Logger - a file already exits at path \"%s\" and overwrite flag was not set", path);

	file = fopen(path, truncate ? "wb" : "ab");

	if (!file)
		throw make_error("machine::Logger - unable to open file for %s at \"%s\"", truncate ? "wb" : "ab", path);

	_tzset();
}

machine::Logger::~Logger()
{
	if(file) fclose(file);
}

void machine::Logger::out(char const * format, ...) const
{
	va_list args;
	va_start(args, format);

	fwrite_time(file, true, false);
	fputs(" | ", file);
	__mingw_fprintf(file, "%05d | ", lineno);
	__mingw_vfprintf(file, format, args);
	fputs("\n", file);

	va_end(args);
}



void machine::Vars::assign(char const * key, int op, unsigned long long val)
{
	unsigned long long const old_val = data[key];
	printf(" {var '%s': %llu} ", key, old_val);

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
			// printf("%s = ", context.dst);
			read(context);
			vars.assign(context.dst, context.ao, acc);
			break;
	}
}

void read_from_file(machine::Context const & context, machine::State & state)
{
	unsigned long long * const out = *context.dst ? &state.vars.data[context.dst] : nullptr;

	switch (context.dtyp)
	{
		default: throw make_error("not implemented: read_from_file operation for dtyp %d", context.dtyp);

		case machine::Context::DT_FIXED32:
			// printf(" FIXED32");
			machine::read_fixed32(state.file, context.dtyp, context.styp, out);
			break;
	}
}

void read_from_mem(machine::Context const & context, machine::State & state)
{
	throw make_error("not implemented: read_from_mem");
}

typedef void (* ReadMethod) (machine::Context const &, machine::State &);

ReadMethod get_read_method(machine::Context const & context)
{
	switch (context.readop)
	{
		default: throw make_error("not implemented: read operation %d", context.readop);

		case machine::Context::R_D:
		case machine::Context::R_I:
			return read_from_file;

		case machine::Context::R_DFI:
		case machine::Context::R_IFI:
			return read_from_mem;
	}
}

void machine::State::read(Context const & context)
{
	auto read_method = get_read_method(context);

	// printf("READ");
	read_method(context, *this);
	// switch (context.styp)
	// {
	// 	case Context::ST_LE: break;
	// 	case Context::ST_BE: printf(" AS BigEndian"); break;
	// }
	// printf("\n");
}

void machine::State::yield_si(char const * str, char const * key)
{
	log("yield_si '%s' %lld", str, vars.data[key]);
}

void machine::read_fixed32(FILE * file, int dtyp, int styp, unsigned long long * out)
{
	unsigned long s32 = 0;
	fread(&s32, 4, 1, file);
	if (!out) return;
	if (Context::ST_BE == styp) flip_32(out, &s32);
	else *out = (unsigned long long) s32;
}

void machine::flip_32(unsigned long long * dst, unsigned long const * src)
{
	// TODO: ws2_32.dll's htonl() does this using only
	// scratch registers .

	auto d = (char *) dst;
	auto s = (char const *) src;

	d[0] = s[3];
	d[1] = s[2];
	d[2] = s[1];
	d[3] = s[0];
}
