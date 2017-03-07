%{
#include "pb.h"
#include "pb.tab.h"
int start_token;
%}

%%

%{
  if (start_token) {
    int t = start_token;
    start_token = 0;
    return t;
  }
%}

"="      { return EQ; }
"<>"     { return NE; }
"≠"      { return NE; }
"<"      { return LT; }
"<="     { return LE; }
"≤"      { return LE; }
">"      { return GT; }
">="     { return GE; }
"≥"      { return GE; }
"+"      { return PLUS; }
"-"      { return MINUS; }
"*"      { return MUL; }
"/"      { return DIV; }
"^"      { return POW; }
"↑"      { return POW; }
")"      { return RPAREN; }
"("      { return LPAREN; }
"SIN"    { return SIN; }
"COS"    { return COS; }
"TAN"    { return TAN; }
"ASN"    { return ASN; }
"ACS"    { return ACS; }
"ATN"    { return ATN; }
"SQR"    { return SQR; }
"EXP"    { return EXP; }
"LN"     { return LN; }
"LOG"    { return LOG; }
"INT"    { return INT; }
"FRAC"   { return FRAC; }
"ABS"    { return ABS; }
"SGN"    { return SGN; }
"RND"    { return RND; }
"RAN"[ ]*"#"   { return RAN; }
"PI"     { return PI; }
"π"      { return PI; }
"𝜋"      { return PI; }
";"      { return SEMICOLON; }
":"      { return COLON; }
","      { return COMMA; }
"#"      { return HASH; }
"INPUT"  { return INPUT; }
"KEY"    { return KEY; }
"PRINT"  { return PRINT; }
"CSR"    { return CSR; }
"GOTO"   { return GOTO; }
"IF"     { return IF; }
"THEN"   { return THEN; }
"GOSUB"  { return GOSUB; }
"RETURN" { return RETURN; }
"FOR"    { return FOR; }
"TO"     { return TO; }
"STEP"   { return STEP; }
"NEXT"   { return NEXT; }
"STOP"   { return STOP; }
"END"    { return END; }
"VAC"    { return VAC; }
"LIST"   { return LIST; }
"LIST"[ ]*"A" { return LISTA; }
"RUN"    { return RUN; }
"CLEAR"  { return CLEAR; }
"CLEAR"[ ]*"A" { return CLEARA; }
"MODE"   { return MODE; }
"SET"[ ]*"E" { return SETE; }
"SET"[ ]*"F" { return SETF; }
"SET"[ ]*"N" { return SETN; }
"LEN"    { return LEN; }
"MID"    { return MID; }
"VAL"    { return VAL; }
"SAVE"   { return SAVE; }
"LOAD"   { return LOAD; }
"SAVE"[ ]*"A" { return SAVEA; }
"LOAD"[ ]*"A" { return LOADA; }
"PUT"    { return PUT; }
"GET"    { return GET; }
"VER"    { return VER; }

"DEFM"   { return DEFM; }

"\n"+    { return EOLN; }

P[0-9]    { yylval.intValue = atoi(yytext + 1); return PROGRAM; }

[0-9]+   { yylval.intValue = atoi(yytext); return INTEGER; }
[0-9.]+  { yylval.floatValue = atof(yytext); return FLOAT; }
[0-9.]+E[0-9]+ { yylval.floatValue = atof(yytext); return FLOAT; }
\"[ !#-~◀_≤≠≥←↓→π○◻▵×÷ᴇ▪°♠♥♦♣█ΣΩμ\\]*\"  { yylval.stringValue = yytext; return STRING; }
[A-Z]"$" { yylval.intValue = yytext[0] - 'A'; return STRVAR; }
"$"      { yylval.intValue = -1; return STRVAR; }
[A-Z]    { yylval.intValue = yytext[0] - 'A'; return NAME; }
\        { ; }
\t       { ; }

"Program:"      { return PF; }
"All programs:" { return AF; }
"END"           { return END; }

.        { yyerror("illegal token"); }
%%
#ifndef yywrap
int yywrap() { return 1; }
#endif
