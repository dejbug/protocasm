%{
#include <stdio.h>
#include <string.h>

int yylex();
void yyerror(char const *);

extern int yylineno;

enum DataType {
	t_null,
	t_string,
	t_bytes,
	t_fixed32,
	t_int32,
};

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
	int styp;
}

/* declare tokens */
%token <id> IDENTIFIER
%token <u64> NUMBER
%token O_LBRACKET O_RBRACKET O_COLON O_ASSIGN O_ADDA O_SUBA
%token K_AS K_IN K_EQ K_NE K_IF K_ELSE K_THEN K_FAIL K_WARN K_READ
%token K_SKIP K_MARK K_MATCH K_RESET K_BYTES
%token T_STRING T_BYTES T_FIXED32 T_INT32
%token S_BE S_LE
%token EOL

%type <dtyp> datatype
%type <styp> stortype

%%

lines: /* nothing -- matches at beginning of input */
|	lines line EOL { printf("\n  %03d  ", ++yylineno); } /* EOL is end of an expression */
;

line: /* nothing -- matches empty lines */
|	IDENTIFIER O_ASSIGN NUMBER { printf("%s = %d", $1, $3); }
|	IDENTIFIER O_ASSIGN readop { printf("%s = ", $1); }
|	IDENTIFIER O_COLON { printf("label (%s)", $1); }
|	arithop
|	readop
|	readop K_ELSE signal
|	K_IF K_MATCH NUMBER readop
|	K_IF K_MATCH NUMBER K_SKIP datatype
|	K_MATCH NUMBER { printf("match(#%02x)", $2); }
|	K_MARK NUMBER
|	K_MARK NUMBER ops IDENTIFIER K_ELSE signal
;

arithop:
	IDENTIFIER O_ADDA NUMBER
|	IDENTIFIER O_ADDA IDENTIFIER
;

readop:
	K_READ datatype { printf("read(%d)", $2); }
|	K_READ datatype K_AS IDENTIFIER { printf("%s = read(%d)", $4, $2); }
|	K_READ datatype K_IN stortype K_AS IDENTIFIER { printf("%s = *(%d*) read(%d)", $6, $4, $2); }
;

datatype:
	T_STRING { $$ = t_string; }
|	T_BYTES { $$ = t_bytes; }
|	T_FIXED32 { $$ = t_fixed32; }
|	T_INT32 { $$ = t_int32; }
;

stortype:
	S_BE { $$ = s_be; }
|	S_LE { $$ = s_le; }
;

ops: K_EQ | K_NE ;

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
