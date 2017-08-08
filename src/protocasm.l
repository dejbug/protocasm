%option noyywrap
%option nodefault

%{
#include "protocasm.tab.h"
%}

%%


[=:{}] { return *yytext; }
"+=" { return O_ADDA; }
"-=" { return O_SUBA; }

"=="|EQ|EQU { return K_EQ; }
"!="|NE|NEQ { return K_NE; }
">="|GE|GEQ { return K_GE; }

"AS" { return K_AS; }
"IF" { return K_IF; }
"ELSE" { return K_ELSE; }
"FAIL" { return K_FAIL; }
"WARN" { return K_WARN; }
"READ" { return K_READ; }
"SKIP" { return K_SKIP; }
"FROM" { return K_FROM; }
"GOTO" { return K_GOTO; }
"MARK" { return K_MARK; }
"DUMP" { return K_DUMP; }
"MATCH" { return K_MATCH; }
"RESET" { return K_RESET; }
"YIELD" { return K_YIELD; }

"KEY" { return T_KEY; }
"STRING" { return T_STRING; }
"BYTES" { return T_BYTES; }
"FIXED32" { return T_FIXED32; }
"FIXED32_BE" { return T_FIXED32_BE; }
"INT32" { return T_INT32; }

"LittleEndian" { return S_LE; }
"BigEndian" { return S_BE; }

"0"[0-9A-F]+ { yylval.u64 = strtoul(yytext, NULL, 16); return NUMBER; }
"0"|[1-9][0-9]* { yylval.u64 = strtoul(yytext, NULL, 10); return NUMBER; }
\n { ++yylineno; return EOL; }

#.* { /* ignore comment lines */ }
[ \t] { /* ignore whitespace */ }
. { printf("Mystery character %c\n", *yytext); }

[_a-z][_a-z0-9]* { strncpy(yylval.id, yytext, sizeof(yylval.id)-1); return IDENTIFIER; }
"\""[^"]*"\"" { strncpy(yylval.id, yytext, sizeof(yylval.id)-1); return STRING; }

%%

