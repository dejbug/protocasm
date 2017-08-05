%option noyywrap

%{
#include "protocasm.tab.h"
%}

%%


":" { return O_COLON; }
"[" { return O_LBRACKET; }
"]" { return O_RBRACKET; }

"AS" { return K_AS; }
"IN" { return K_IN; }
"EQ" { return K_EQ; }
"NE" { return K_NE; }
"ELSE" { return K_ELSE; }
"FAIL" { return K_FAIL; }
"WARN" { return K_WARN; }
"READ" { return K_READ; }
"FLIP" { return K_FLIP; }
"MARK" { return K_MARK; }
"MATCH" { return K_MATCH; }
"RESET" { return K_RESET; }

"string" { return T_STRING; }
"fixed32" { return T_FIXED32; }
"int32" { return T_INT32; }

"BigEndian" { return S_BE; }
"LittleEndian" { return S_LE; }

[0-9A-Z]+ { yylval.u64 = strtoul(yytext, NULL, 16); return NUMBER; }
\n { return EOL; }

#.* { /* ignore comment lines */ }
[ \t] { /* ignore whitespace */ }
. { printf("Mystery character %c\n", *yytext); }

[_a-z][_a-z0-9]* { strncpy(yylval.id, yytext, sizeof(yylval.id)-1); return IDENTIFIER; }

%%

