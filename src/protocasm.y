%{
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <crtdbg.h>

#ifdef _DEBUG
extern "C" __declspec(dllimport) void __stdcall OutputDebugStringA (char const *);
#endif

#include "machine.h"
machine::State state;
machine::Context context;
void dbgout(char const *, ...);

int yylex();
void yyerror(char const *, ...);

extern int yylineno;
int dbgout_lineno = 0;
%}

%union {
	char id[255 + 1];
	unsigned long long u64;
	char text[1024];
}

/* declare tokens */
%token <id> IDENTIFIER
%token <u64> NUMBER
%token <id> STRING
%token O_ADDA O_SUBA
%token K_EQ K_NE K_GE
%token K_AS K_IF K_ELSE K_FAIL K_WARN K_READ K_SKIP K_FROM K_OPEN
%token K_MARK K_MATCH K_RESET K_BYTES K_YIELD K_GOTO K_DUMP
%token T_KEY T_FIXED32 T_STRING T_BYTES T_INT32
%token S_LE S_BE
%token EOL

%%

lines:
	/* nothing */ {
		dbgout("%03d lines: /* nothing */", dbgout_lineno++);
		printf("\n  %03d  ", yylineno /* == 1 */);
	}
|	lines line EOL {
		dbgout("%03d lines: lines line EOL", dbgout_lineno++);
		printf("\n  %03d  ", yylineno/* > 1 */);
	}
;

line:
	/* nothing -- matches empty lines */ {
		dbgout("%03d line: /* nothing */", dbgout_lineno);
	}
|	expr {
		dbgout("%03d line: expr", dbgout_lineno);
	}
|	expr K_IF condop {
		dbgout("%03d line: expr K_IF condop", dbgout_lineno);
	}
|	IDENTIFIER ':' {
		dbgout("%03d line: IDENTIFIER ':'", dbgout_lineno);
		state.labels.set($1, yylineno);
	}
|	K_DUMP {
		dbgout("%03d line: K_DUMP", dbgout_lineno);
		state.dump();
	}

|	K_IF K_MATCH NUMBER readop
|	K_IF K_MATCH NUMBER K_SKIP datatype
;

expr:
	K_OPEN STRING {
		dbgout("%03d expr: K_OPEN STRING", dbgout_lineno);
		state.open($2);
	}
|	K_GOTO IDENTIFIER {
		dbgout("%03d expr: K_GOTO IDENTIFIER", dbgout_lineno);
	}
|	yieldexpr {
		dbgout("%03d expr: yieldexpr", dbgout_lineno);
	}
|	assignment {
		dbgout("%03d expr: assignment", dbgout_lineno);
		state.assignment(context);
	}
|	signal {
		dbgout("%03d expr: signal", dbgout_lineno);
	}
|	K_SKIP NUMBER {
		dbgout("%03d expr: K_SKIP NUMBER", dbgout_lineno);
	}
|	K_SKIP IDENTIFIER {
		dbgout("%03d expr: K_SKIP IDENTIFIER", dbgout_lineno);
	}
|	K_MATCH NUMBER {
		dbgout("%03d expr: K_MATCH NUMBER", dbgout_lineno);
	}
;

yieldexpr:
	K_YIELD STRING NUMBER
|	K_YIELD STRING IDENTIFIER {
		dbgout("%03d yieldexpr: K_YIELD STRING IDENTIFIER", dbgout_lineno);
		state.yield_si($2, $3);
	}
|	K_YIELD NUMBER
|	K_YIELD IDENTIFIER
|	K_YIELD STRING
;

condop:
	K_MATCH NUMBER
|	K_MATCH IDENTIFIER
|	IDENTIFIER ops NUMBER
|	IDENTIFIER ops IDENTIFIER
|	K_MARK K_FROM IDENTIFIER ops NUMBER
|	K_MARK K_FROM IDENTIFIER ops IDENTIFIER
;

assignment:
	IDENTIFIER assignop NUMBER {
		dbgout("%03d assignment: IDENTIFIER assignop NUMBER", dbgout_lineno);
		context.at = machine::Context::AT_IN;
		snprintf(context.dst, sizeof(context.dst), $1);
		context.val = $3;
	}
|	IDENTIFIER assignop IDENTIFIER {
		dbgout("%03d assignment: IDENTIFIER assignop IDENTIFIER", dbgout_lineno);
		context.at = machine::Context::AT_II;
		snprintf(context.dst, sizeof(context.dst), $1);
		snprintf(context.src, sizeof(context.src), $3);
	}
|	IDENTIFIER assignop readop {
		dbgout("%03d assignment: IDENTIFIER assignop readop", dbgout_lineno);
		context.at = machine::Context::AT_IR;
		snprintf(context.dst, sizeof(context.dst), $1);
	}
;

readop:
	readop1 {
		dbgout("%03d readop: readop1", dbgout_lineno);
		context.styp = machine::Context::ST_LE;
	}
|	readop1 K_AS stortype {
		dbgout("%03d readop: readop1 K_AS stortype", dbgout_lineno);
	}
;

readop1:
	K_READ datatype {
		dbgout("%03d readop1: K_READ datatype", dbgout_lineno);
		context.ao = machine::Context::AO_NONE;
		context.readop = machine::Context::R_D;
	}
|	K_READ datatype K_FROM IDENTIFIER {
		dbgout("%03d readop1: K_READ datatype K_FROM IDENTIFIER", dbgout_lineno);
		context.ao = machine::Context::AO_NONE;
		context.readop = machine::Context::R_DFI;
		snprintf(context.src, sizeof(context.src), $4);
	}
|	K_READ IDENTIFIER {
		dbgout("%03d readop1: K_READ IDENTIFIER", dbgout_lineno);
		context.ao = machine::Context::AO_NONE;
		context.readop = machine::Context::R_I;
		snprintf(context.rid, sizeof(context.rid), $2);
	}
|	K_READ IDENTIFIER K_FROM IDENTIFIER {
		dbgout("%03d readop1: K_READ IDENTIFIER K_FROM IDENTIFIER", dbgout_lineno);
		context.ao = machine::Context::AO_NONE;
		context.readop = machine::Context::R_IFI;
		snprintf(context.rid, sizeof(context.rid), $2);
		snprintf(context.src, sizeof(context.src), $4);
	}
;

datatype:
	T_KEY {
		dbgout("%03d datatype: T_KEY", dbgout_lineno);
		context.dtyp = machine::Context::DT_KEY;
	}
|	T_STRING {
		dbgout("%03d datatype: T_STRING", dbgout_lineno);
		context.dtyp = machine::Context::DT_STRING;
	}
|	T_BYTES {
		dbgout("%03d datatype: T_BYTES", dbgout_lineno);
		context.dtyp = machine::Context::DT_BYTES;
	}
|	T_FIXED32 {
		dbgout("%03d datatype: T_FIXED32", dbgout_lineno);
		context.dtyp = machine::Context::DT_FIXED32;
	}
|	T_INT32 {
		dbgout("%03d datatype: T_INT32", dbgout_lineno);
		context.dtyp = machine::Context::DT_INT32;
	}
// ...
;

assignop:
	'=' {
		dbgout("%03d assignop: '='", dbgout_lineno);
		context.ao = machine::Context::AO_ASS;
	}
|	O_ADDA {
		dbgout("%03d assignop: O_ADDA", dbgout_lineno);
		context.ao = machine::Context::AO_ADD;
	}
|	O_SUBA {
		dbgout("%03d assignop: O_SUBA", dbgout_lineno);
		context.ao = machine::Context::AO_SUB;
	}
// | ...
;

stortype:
	S_LE {
		dbgout("%03d stortype: S_LE", dbgout_lineno);
		context.styp = machine::Context::ST_LE;
	}
|	S_BE {
		dbgout("%03d stortype: S_BE", dbgout_lineno);
		context.styp = machine::Context::ST_BE;
	}
;

ops: K_EQ | K_NE | K_GE ;

signal: K_FAIL | K_WARN ;

%%

int main(int argc, char **argv)
{
	extern FILE * yyin;

	if(argc < 2) return 1;

	if(!(yyin = fopen(argv[1], "r")))
	{
		perror(argv[1]);
		return 2;
	}

	try
	{
		yyparse();
	}

	catch (std::runtime_error & e)
	{
		yyerror(e.what());
		return 0;
	}

	return 0;
}

void yyerror(char const * format, ...)
{
	va_list args;
	va_start(args, format);
	__mingw_fprintf(stderr, "<error: ");
	__mingw_vfprintf(stderr, format, args);
	__mingw_fprintf(stderr, ">\n");
	va_end(args);
}

void dbgout(char const * format, ...)
{
#ifdef _DEBUG
	size_t const N = 1024;
	char buffer[N + 1] = {0};

	va_list args;
	va_start(args, format);
	__mingw_vsnprintf(buffer, N, format, args);
	va_end(args);

	// _RPT0(_CRT_WARN, buffer);
	OutputDebugStringA(buffer);
#endif
}
