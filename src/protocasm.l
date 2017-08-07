%option noyywrap

%{
#include "protocasm.tab.h"
%}

%%


"=" { return O_ASSIGN; }
"+=" { return O_ADDA; }
"-=" { return O_SUBA; }
":" { return O_COLON; }
"[" { return O_LBRACKET; }
"]" { return O_RBRACKET; }

"EQ" { return K_EQ; }
"NE" { return K_NE; }
"GE" { return K_GE; }

"AS" { return K_AS; }
"IN" { return K_IN; }
"IF" { return K_IF; }
"ELSE" { return K_ELSE; }
"FAIL" { return K_FAIL; }
"WARN" { return K_WARN; }
"READ" { return K_READ; }
"SKIP" { return K_SKIP; }
"FROM" { return K_FROM; }
"GOTO" { return K_GOTO; }
"MARK" { return K_MARK; }
"MATCH" { return K_MATCH; }
"RESET" { return K_RESET; }
"YIELD" { return K_YIELD; }

"KEY" { return T_KEY; }
"STRING" { return T_STRING; }
"BYTES" { return T_BYTES; }
"FIXED32" { return T_FIXED32; }
"FIXED32_BE" { return T_FIXED32_BE; }
"INT32" { return T_INT32; }

"0"[0-9A-F]+ { yylval.u64 = strtoul(yytext, NULL, 16); return NUMBER; }
"0"|[1-9][0-9]* { yylval.u64 = strtoul(yytext, NULL, 10); return NUMBER; }
\n { return EOL; }

#.* { /* ignore comment lines */ }
[ \t] { /* ignore whitespace */ }
. { printf("Mystery character %c\n", *yytext); }

[_a-z][_a-z0-9]* { strncpy(yylval.id, yytext, sizeof(yylval.id)-1); return IDENTIFIER; }
"\""[^"]*"\"" { strncpy(yylval.id, yytext, sizeof(yylval.id)-1); return STRING; }

%%

