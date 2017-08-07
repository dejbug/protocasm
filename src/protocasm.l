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

"AS" { return K_AS; }
"IN" { return K_IN; }
"EQ" { return K_EQ; }
"NE" { return K_NE; }
"IF" { return K_IF; }
"ELSE" { return K_ELSE; }
"THEN" { return K_THEN; }
"FAIL" { return K_FAIL; }
"WARN" { return K_WARN; }
"READ" { return K_READ; }
"SKIP" { return K_SKIP; }
"MARK" { return K_MARK; }
"MATCH" { return K_MATCH; }
"RESET" { return K_RESET; }

"string" { return T_STRING; }
"bytes" { return T_BYTES; }
"fixed32" { return T_FIXED32; }
"int32" { return T_INT32; }

"BigEndian" { return S_BE; }
"LittleEndian" { return S_LE; }

"0"[0-9A-F]+ { yylval.u64 = strtoul(yytext, NULL, 16); return NUMBER; }
"0"|[1-9][0-9]* { yylval.u64 = strtoul(yytext, NULL, 10); return NUMBER; }
\n { return EOL; }

#.* { /* ignore comment lines */ }
[ \t] { /* ignore whitespace */ }
. { printf("Mystery character %c\n", *yytext); }

[_a-z][_a-z0-9]* { strncpy(yylval.id, yytext, sizeof(yylval.id)-1); return IDENTIFIER; }

%%

