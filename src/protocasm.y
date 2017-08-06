%{
#include <stdio.h>
#include <string.h>

int yylex();
void yyerror(char const *);

extern int yylineno;

enum DataType {
	t_null,
	t_string,
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
%token O_COLON O_LBRACKET O_RBRACKET
%token K_AS K_IN K_EQ K_NE K_ELSE K_FAIL K_WARN K_READ
%token K_MARK K_MATCH K_RESET K_BYTES
%token T_STRING T_FIXED32 T_INT32
%token S_BE S_LE
%token EOL

%type <dtyp> datatype
%type <styp> stortype

%%

lines: /* nothing -- matches at beginning of input */
|	lines line EOL { ++yylineno; } /* EOL is end of an expression */
;
line: /* nothing -- matches empty lines */
|	K_MATCH NUMBER { printf(" %03d  match(#%02x)\n", yylineno, $2); }
|	K_READ datatype { printf(" %03d  read(%d)\n", yylineno, $2); }
|	K_READ datatype K_AS IDENTIFIER { printf(" %03d  %s = read(%d)\n", yylineno, $4, $2); }
|	K_READ datatype K_IN stortype K_AS IDENTIFIER { printf(" %03d  %s = *(%d*) read(%d)\n", yylineno, $6, $4, $2); }
|	K_MARK K_RESET
|	K_MARK ops IDENTIFIER K_ELSE signal
;
datatype:
	T_STRING { $$ = t_string; }
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
	fprintf(stderr, "! %s at line %d\n", s, yylineno);
}
