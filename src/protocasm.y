%{
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <crtdbg.h>

int yylex();
void yyerror(char const *, ...);

#include "machine.h"
machine::State state;
machine::Context context;
machine::Logger logger("protocasm.log");

extern int yylineno;
%}

%union {
	char id[255 + 1];
	unsigned long long u64;
	char text[1024];
}

/* declare tokens */
%token <id> IDENTIFIER
%token <u64> NUMBER
%token <text> STRING
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
		logger.out("lines: /* nothing */");
		logger.inc();
		// echo("\n  %03d  ", yylineno /* == 1 */);
		printf("\n  %03d  ", yylineno /* == 1 */);
	}
|	lines line EOL {
		logger.out("lines: lines line EOL");
		logger.inc();
		// echo("\n  %03d  ", yylineno/* > 1 */);
		printf("\n  %03d  ", yylineno/* > 1 */);
	}
;

line:
	/* nothing -- matches empty lines */ {
		logger.out("line: /* nothing */");
	}
|	expr {
		logger.out("line: expr");
	}
|	expr K_IF condop {
		logger.out("line: expr K_IF condop");
	}
|	IDENTIFIER ':' {
		logger.out("line: IDENTIFIER ':'");
		state.labels.set($1, yylineno);
	}
|	K_DUMP {
		logger.out("line: K_DUMP");
		state.dump();
	}

|	K_IF K_MATCH NUMBER readop
|	K_IF K_MATCH NUMBER K_SKIP datatype
;

expr:
	K_OPEN STRING {
		logger.out("expr: K_OPEN STRING");
		state.open($2);
	}
|	K_GOTO IDENTIFIER {
		logger.out("expr: K_GOTO IDENTIFIER");
	}
|	yieldexpr {
		logger.out("expr: yieldexpr");
	}
|	assignment {
		logger.out("expr: assignment");
		state.assignment(context);
	}
|	signal {
		logger.out("expr: signal");
	}
|	K_SKIP NUMBER {
		logger.out("expr: K_SKIP NUMBER");
	}
|	K_SKIP IDENTIFIER {
		logger.out("expr: K_SKIP IDENTIFIER");
	}
|	K_MATCH NUMBER {
		logger.out("expr: K_MATCH NUMBER");
	}
;

yieldexpr:
	K_YIELD STRING NUMBER
|	K_YIELD STRING IDENTIFIER {
		logger.out("yieldexpr: K_YIELD STRING IDENTIFIER");
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
		logger.out("assignment: IDENTIFIER assignop NUMBER");
		context.at = machine::Context::AT_IN;
		snprintf(context.dst, sizeof(context.dst), $1);
		context.val = $3;
	}
|	IDENTIFIER assignop IDENTIFIER {
		logger.out("assignment: IDENTIFIER assignop IDENTIFIER");
		context.at = machine::Context::AT_II;
		snprintf(context.dst, sizeof(context.dst), $1);
		snprintf(context.src, sizeof(context.src), $3);
	}
|	IDENTIFIER assignop readop {
		logger.out("assignment: IDENTIFIER assignop readop");
		context.at = machine::Context::AT_IR;
		snprintf(context.dst, sizeof(context.dst), $1);
	}
;

readop:
	readop1 {
		logger.out("readop: readop1");
		context.styp = machine::Context::ST_LE;
	}
|	readop1 K_AS stortype {
		logger.out("readop: readop1 K_AS stortype");
	}
;

readop1:
	K_READ datatype {
		logger.out("readop1: K_READ datatype");
		context.readop = machine::Context::R_D;
	}
|	K_READ datatype K_FROM IDENTIFIER {
		logger.out("readop1: K_READ datatype K_FROM IDENTIFIER");
		context.readop = machine::Context::R_DFI;
		snprintf(context.src, sizeof(context.src), $4);
	}
|	K_READ IDENTIFIER {
		logger.out("readop1: K_READ IDENTIFIER");
		context.readop = machine::Context::R_I;
		snprintf(context.rid, sizeof(context.rid), $2);
	}
|	K_READ IDENTIFIER K_FROM IDENTIFIER {
		logger.out("readop1: K_READ IDENTIFIER K_FROM IDENTIFIER");
		context.readop = machine::Context::R_IFI;
		snprintf(context.rid, sizeof(context.rid), $2);
		snprintf(context.src, sizeof(context.src), $4);
	}
;

datatype:
	T_KEY {
		logger.out("datatype: T_KEY");
		context.dtyp = machine::Context::DT_KEY;
	}
|	T_STRING {
		logger.out("datatype: T_STRING");
		context.dtyp = machine::Context::DT_STRING;
	}
|	T_BYTES {
		logger.out("datatype: T_BYTES");
		context.dtyp = machine::Context::DT_BYTES;
	}
|	T_FIXED32 {
		logger.out("datatype: T_FIXED32");
		context.dtyp = machine::Context::DT_FIXED32;
	}
|	T_INT32 {
		logger.out("datatype: T_INT32");
		context.dtyp = machine::Context::DT_INT32;
	}
// ...
;

assignop:
	'=' {
		logger.out("assignop: '='");
		context.ao = machine::Context::AO_ASS;
	}
|	O_ADDA {
		logger.out("assignop: O_ADDA");
		context.ao = machine::Context::AO_ADD;
	}
|	O_SUBA {
		logger.out("assignop: O_SUBA");
		context.ao = machine::Context::AO_SUB;
	}
// | ...
;

stortype:
	S_LE {
		logger.out("stortype: S_LE");
		context.styp = machine::Context::ST_LE;
	}
|	S_BE {
		logger.out("stortype: S_BE");
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
