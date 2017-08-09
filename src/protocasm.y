%{
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "machine.h"

int yylex();
void yyerror(char const *);
// void emit(char const *, ...);

extern int yylineno;

enum StorageType {
	s_null,
	s_le,
	s_be,
};

machine::State state;

%}

%union {
	char id[255+1];
	unsigned long long u64;
	int dtyp;
	int styp;
	int aop;
	char text[1024];
	char text1[1024];
}

/* declare tokens */
%token <id> IDENTIFIER
%token <u64> NUMBER
%token <id> STRING
%token O_ADDA O_SUBA
%token K_EQ K_NE K_GE
%token K_AS K_IF K_ELSE K_FAIL K_WARN K_READ K_SKIP K_FROM K_OPEN
%token K_MARK K_MATCH K_RESET K_BYTES K_YIELD K_GOTO K_DUMP
%token T_KEY T_FIXED32 T_FIXED32_BE T_STRING T_BYTES T_INT32
%token S_LE S_BE
%token EOL

%type <text> expr
%type <dtyp> datatype
%type <styp> stortype
%type <aop> assignop
%type <text1> readop1
%type <text> readop
%type <text> assignment

%%

lines:
	/* nothing */ { printf("\n  %03d  ", yylineno /* == 1 */); }
|	lines line EOL { printf("\n  %03d  ", yylineno/* > 1 */); }
;

line:
	/* nothing -- matches empty lines */
|	expr { printf("! %s", $1); }
|	expr K_IF condop { printf("? %s", $1); }
|	IDENTIFIER ':' { printf("  [label] %s", $1); }
|	K_DUMP { printf("  [dump]\n"); state.vars.dump(); }

|	K_IF K_MATCH NUMBER readop
|	K_IF K_MATCH NUMBER K_SKIP datatype
|	K_MATCH NUMBER { __mingw_printf("match(#%02llx)", $2); }
|	K_MARK NUMBER
|	K_MARK NUMBER ops IDENTIFIER K_ELSE signal
|	K_MATCH NUMBER readop
;

expr:
	K_OPEN STRING { state.open($2); }
|	K_GOTO IDENTIFIER { snprintf($$, sizeof($$), "[jump] %s", $2); }
|	yieldexpr { snprintf($$, sizeof($$), "[yield]"); }
|	assignment { $$[0] = 0; }
|	signal { snprintf($$, sizeof($$), "[signal]"); }
|	K_SKIP NUMBER { __mingw_snprintf($$, sizeof($$), "[skip] %lld", $2); }
|	K_SKIP IDENTIFIER { snprintf($$, sizeof($$), "[skip] %s", $2); }
;

yieldexpr:
	K_YIELD STRING NUMBER
|	K_YIELD STRING IDENTIFIER
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
	IDENTIFIER assignop NUMBER { state.vars.assign($1, $2, $3); }
|	IDENTIFIER assignop IDENTIFIER { state.vars.assign($1, $2, $3); }
|	IDENTIFIER assignop readop { $$[0] = 0; }
;

assignop:
	'=' { $$ = '='; }
|	O_ADDA { $$ = O_ADDA; }
|	O_SUBA { $$ = O_SUBA; }
// | ...
;

readop:
	readop1 { strncpy($$, $1, sizeof($$)); }
|	readop1 K_AS stortype { snprintf($$, sizeof($$), "{%d} %s", $3, $1); }
;

readop1:
	K_READ datatype { snprintf($$, sizeof($$), "read(%d)", $2); }
|	K_READ datatype K_FROM IDENTIFIER { snprintf($$, sizeof($$), "%s.read(%d)", $4, $2); }
|	K_READ IDENTIFIER { snprintf($$, sizeof($$), "read(%s)", $2); }
|	K_READ IDENTIFIER K_FROM IDENTIFIER { snprintf($$, sizeof($$), "%s.read(%s)", $4, $2); }
;

datatype:
	T_KEY { $$ = T_KEY; }
|	T_STRING { $$ = T_STRING; }
|	T_BYTES { $$ = T_BYTES; }
|	T_FIXED32 { $$ = T_FIXED32; }
|	T_FIXED32_BE { $$ = T_FIXED32_BE; }
|	T_INT32 { $$ = T_INT32; }
// ...
;

stortype:
	S_LE { $$ = S_LE; }
|	S_BE { $$ = S_BE; }
;

ops: K_EQ | K_NE | K_GE ;

signal: K_FAIL | K_WARN ;

%%

// void emit(char const * s, ...)
// {
// 	va_list ap;
// 	va_start(ap, s);
// 	vfprintf(stdout, s, ap);
// 	printf("\n");
// }

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
		return 3;
	}

	return 0;
}

void yyerror(char const * s)
{
	fprintf(stderr, "<%s>\n", s);
}
