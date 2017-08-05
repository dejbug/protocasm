%{
#include <stdio.h>
#include <string.h>
int yylex();
void yyerror(char const *, ...);
extern int yylineno;

enum Typ {
	t_string = 1,
	t_fixed32,
	t_int32,
};
%}

%union {
	char id[64+1];
	unsigned long long u64;
	int typ;
}

/* declare tokens */
%token <id> IDENTIFIER
%token <u64> NUMBER
%token O_COLON O_LBRACKET O_RBRACKET
%token K_AS K_EQ K_ELSE K_FAIL K_WARN K_READ
%token K_FLIP K_MARK K_MATCH K_RESET K_BYTES
%token T_STRING T_FIXED32 T_INT32
%token EOL

%type <typ> type1
%type <id> type2

%%

lines: /* nothing -- matches at beginning of input */
|	lines line EOL { ++yylineno; } /* EOL is end of an expression */
;
line: /* nothing -- matches empty lines */
|	K_READ type1 { printf(" %03d  read(%d)\n", yylineno, $2); }
|	K_READ type1 K_AS IDENTIFIER { printf(" %03d  %s = read(%d)\n", yylineno, $4, $2); }
|	K_READ type2 { printf(" %03d  read(%s)\n", yylineno, $2); }
|	K_READ type2 K_AS IDENTIFIER { printf(" %03d  %s = read(%s)\n", yylineno, $4, $2); }
|	K_FLIP K_BYTES IDENTIFIER
|	K_MARK K_RESET
|	K_MARK xyz1 IDENTIFIER K_ELSE xyz2
|	K_MATCH NUMBER
;
type1:
	T_STRING { $$ = t_string; }
|	T_FIXED32 { $$ = t_fixed32; }
|	T_INT32 { $$ = t_int32; }
;
type2: O_LBRACKET IDENTIFIER O_RBRACKET { strncpy($$, $2, sizeof($$)-1); } ;
xyz1: K_EQ ;
xyz2: K_FAIL | K_WARN ;

%%

int main(int argc, char **argv)
{
	extern FILE * yyin;
	
	if(argc > 1)
	{
		if(!(yyin = fopen(argv[1], "r")))
		{
			perror(argv[1]);
			return 1;
		}
		
		yyparse();
		return 0;
	}
}

void yyerror(char const * s, ...)
{
	fprintf(stderr, "! %s at line %d\n", s, yylineno);
}
