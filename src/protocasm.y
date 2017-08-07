%{
#include <stdio.h>
#include <string.h>

int yylex();
void yyerror(char const *);

extern int yylineno;

enum StorageType {
	s_null,
	s_le,
	s_be,
};
%}

%union {
	char id[255+1];
	unsigned long long u64;
	int dtyp;
	char const * aop;
	char rop[255+1];
}

/* declare tokens */
%token <id> IDENTIFIER
%token <u64> NUMBER
%token <id> STRING
%token O_LBRACKET O_RBRACKET O_COLON O_ASSIGN O_ADDA O_SUBA
%token K_EQ K_NE K_GE
%token K_AS K_IN K_IF K_ELSE K_FAIL K_WARN K_READ K_SKIP K_FROM
%token K_MARK K_MATCH K_RESET K_BYTES K_YIELD K_GOTO
%token T_KEY T_FIXED32 T_FIXED32_BE T_STRING T_BYTES T_INT32
%token EOL

%type <dtyp> datatype
%type <aop> assignop
%type <rop> readop

%%

lines:
	/* nothing -- matches at beginning of input */ { printf("\n  %03d  ", yylineno); }
|	lines line EOL { printf("\n  %03d  ", ++yylineno); } /* EOL is end of an expression */
;

line:
	/* nothing -- matches empty lines */
|	K_GOTO IDENTIFIER { printf("[jump] %s", $2); }
|	K_GOTO IDENTIFIER K_IF condop { printf("[jump] %s", $2); }
|	yieldexpr
|	IDENTIFIER O_COLON { printf("[label] %s", $1); }
|	IDENTIFIER assignop NUMBER { printf("%s %s %d", $1, $2, $3); }
|	IDENTIFIER assignop readop { printf("%s %s %s", $1, $2, $3); }
|	readop
|	signal K_IF condop
|	K_IF K_MATCH NUMBER readop
|	K_IF K_MATCH NUMBER K_SKIP datatype
|	K_MATCH NUMBER { printf("match(#%02x)", $2); }
|	K_MARK NUMBER
|	K_MARK NUMBER ops IDENTIFIER K_ELSE signal
|	K_MATCH NUMBER readop
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

assignop:
	O_ASSIGN { $$ = "="; }
|	O_ADDA { $$ = "+="; }
|	O_SUBA { $$ = "-="; }
;

readop:
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

	yyparse();
	return 0;
}

void yyerror(char const * s)
{
	fprintf(stderr, "! %s\n", s);
}
