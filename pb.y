%{
  #include <stdio.h>
  #include <math.h>
  #include "pb.h"
  #include "Device.h"
  #include "Statement.h"
  
  int curr_prog_area = 0;
  int yydebug = 0;
%}

%union {
  int intValue;
  double floatValue;
  char *stringValue;
  nodeType *node;
  struct StatementTag *statement;
};

%start program

%token EQ
%token NE
%token LT
%token LE
%token GT
%token GE
%token PLUS
%token MINUS
%token MUL
%token DIV
%token POW
%token RPAREN
%token LPAREN
%token SIN
%token COS
%token TAN
%token ASN
%token ACS
%token ATN
%token SQR
%token EXP
%token LN
%token LOG
%token INT
%token FRAC
%token ABS
%token SGN
%token RND
%token RAN
%token PI
%token ASSIGN_NUM
%token ASSIGN_STR
%token SEMICOLON
%token COLON
%token COMMA
%token HASH
%token <intValue> INTEGER
%token <floatValue> FLOAT
%token <stringValue> STRING
%token INPUT
%token KEY
%token PRINT
%token CSR
%token GOTO
%token IF
%token THEN
%token GOSUB
%token RETURN
%token FOR
%token TO
%token STEP
%token NEXT
%token STOP
%token END
%token VAC
%token LIST
%token LISTA
%token RUN
%token CLEAR
%token CLEARA
%token MODE
%token SETE
%token SETF
%token SETN
%token LEN
%token MID
%token VAL
%token SAVE
%token LOAD
%token SAVEA
%token LOADA
%token PUT
%token GET
%token VER
%token DEFM
%token <intValue> NAME
%token <intValue> STRVAR
%token EOLN

%token <intValue> PROGRAM;

%type <node> statement input_expression input_phrase num_assignment str_assignment print_expression char_expression comparison expression pow_expr add_expr mul_expr unary_expr primary integer function_call num_variable str_variable str_literal prog_area

%type <statement> line statement_list;

%%

program:
list

list:
list line   { if ($2) Device_addStatement(device, curr_prog_area, $2); }
|
/* empty */
;

line:
INTEGER statement_list EOLN { $$ = $2; $2->line_num = $1; }
|
PROGRAM EOLN                { $$ = NULL; curr_prog_area = $1; }
;

statement_list:
statement                      { $$ = Statement_create($1, NULL); }
|
statement COLON statement_list { $$ = Statement_create($1, $3); }
;

statement:
INPUT input_expression            { $$ = opr(INPUT, 1, $2); }
|
PRINT                             { $$ = opr(PRINT, 0); }
|
PRINT print_expression        { $$ = opr(PRINT, 1, $2); }
|
PRINT CSR expression SEMICOLON print_expression { $$ = opr(PRINT, 2, $3, $5); }
|
FOR num_assignment TO expression      { $$ = opr(FOR, 2, $2, $4); }
|
FOR num_assignment TO expression STEP expression { $$ = opr(FOR, 3, $2, $4, $6); }
|
NEXT num_variable                 { $$ = opr(NEXT, 1, $2); }
|
STOP                              { $$ = opr(STOP, 0); }
|
END                               { $$ = opr(END, 0); }
|
VAC                               { $$ = opr(VAC, 0); }
|
LIST                              { $$ = opr(LIST, 0); }
|
RUN                               { $$ = opr(RUN, 0); }
|
MODE integer                      { $$ = opr(MODE, 1, $2); }
|
SETE integer                      { $$ = opr(SETE, 1, $2); }
|
SETF integer                      { $$ = opr(SETF, 1, $2); }
|
SETN                              { $$ = opr(SETN, 0); }
|
GOTO expression                   { $$ = opr(GOTO, 1, $2); }
|
GOTO prog_area                     { $$ = opr(GOTO, 1, $2); }
|
GOSUB expression                  { $$ = opr(GOSUB, 1, $2); }
|
GOSUB prog_area                    { $$ = opr(GOSUB, 1, $2); }
|
RETURN                            { $$ = opr(RETURN, 0); }
|
IF comparison SEMICOLON statement { $$ = oprFlags(IF, 1, 2, $2, $4); }
|
IF comparison THEN expression     { $$ = opr(IF, 2, $2, $4); }
|
IF comparison THEN prog_area       { $$ = opr(IF, 2, $2, $4); }
|
num_assignment                    { $$ = $1; }
|
str_assignment                    { $$ = $1; }
;

input_expression:
input_phrase                        { $$ = opr(COMMA, 1, $1); /* $1; */ }
|
input_phrase COMMA input_expression { $$ = opr(COMMA, 2, $1, $3); }
;

input_phrase:
str_literal COMMA num_variable      { $$ = opr(COMMA, 2, $1, $3); }
|
str_literal COMMA str_variable      { $$ = opr(COMMA, 2, $1, $3); }
|
num_variable                        { $$ = opr(COMMA, 2, NULL, $1); }
|
str_variable                        { $$ = opr(COMMA, 2, NULL, $1); }
;

num_assignment:
num_variable EQ expression        { $$ = opr(ASSIGN_NUM, 2, $1, $3); }

str_assignment:
str_variable EQ char_expression { $$ = opr(ASSIGN_STR, 2, $1, $3); }

print_expression:
char_expression { $$ = $1; }
|
char_expression COMMA print_expression { $$ = opr(COMMA, 2, $1, $3); }
|
char_expression SEMICOLON print_expression { $$ = opr(SEMICOLON, 2, $1, $3); }
|
char_expression SEMICOLON { $$ = opr(SEMICOLON, 1, $1); }
;

char_expression:
expression    { $$ = numToStr($1); }
|
str_variable  { $$ = $1; }
|
str_literal   { $$ = $1; }
|
KEY                    { $$ = opr(KEY, 0); }
|
MID LPAREN expression RPAREN      { $$ = opr(MID, 1, $3); }
|
MID LPAREN expression COMMA expression RPAREN { $$ = opr(MID, 2, $3, $5); }
;

comparison:
expression EQ expression { $$ = opr(EQ, 2, $1, $3); }
|
expression NE expression { $$ = opr(NE, 2, $1, $3); }
|
expression LT expression { $$ = opr(LT, 2, $1, $3); }
|
expression LE expression { $$ = opr(LE, 2, $1, $3); }
|
expression GT expression { $$ = opr(GT, 2, $1, $3); }
|
expression GE expression { $$ = opr(GE, 2, $1, $3); }
|
char_expression EQ char_expression { $$ = oprFlags(EQ, 1, 2, $1, $3); }
|
char_expression NE char_expression { $$ = oprFlags(NE, 1, 2, $1, $3); }
|
char_expression LT char_expression { $$ = oprFlags(LT, 1, 2, $1, $3); }
|
char_expression LE char_expression { $$ = oprFlags(LE, 1, 2, $1, $3); }
|
char_expression GT char_expression { $$ = oprFlags(GT, 1, 2, $1, $3); }
|
char_expression GE char_expression { $$ = oprFlags(GE, 1, 2, $1, $3); }
;

expression:
pow_expr { $$ = $1; }
;

pow_expr:
add_expr              { $$ = $1; }
|
pow_expr POW add_expr { $$ = opr(POW, 2, $1, $3); /* pow($1, $3); */ }
;

add_expr:
mul_expr                { $$ = $1; }
|
add_expr PLUS mul_expr  { $$ = opr(PLUS, 2, $1, $3); /* $1 + $3; */ }
|
add_expr MINUS mul_expr { $$ = opr(MINUS, 2, $1, $3); /* $1 - $3; */ }
;

mul_expr:
unary_expr              { $$ = $1; }
|
mul_expr MUL unary_expr { $$ = opr(MUL, 2, $1, $3); /* $1 * $3; */ }
|
mul_expr DIV unary_expr { $$ = opr(DIV, 2, $1, $3); /* $1 / $3; */ }
;

unary_expr:
primary        { $$ = $1; }
|
PLUS primary   { $$ = opr(PLUS, 1, $2); }
|
MINUS primary  { $$ = opr(MINUS, 1, $2); }
;

primary:
function_call            { $$ = $1; }
|
integer                  { $$ = $1; }
|
FLOAT                    { $$ = con($1); }
|
num_variable             { $$ = $1; }
|
LPAREN expression RPAREN { $$ = opr(LPAREN, 1, $2); }
;

integer:
INTEGER { $$ = con($1); }

function_call:
SIN primary           { $$ = opr(SIN, 1, $2); }
|
COS primary           { $$ = opr(COS, 1, $2); }
|
TAN primary           { $$ = opr(TAN, 1, $2); }
|
ASN primary           { $$ = opr(ASN, 1, $2); }
|
ACS primary           { $$ = opr(ACS, 1, $2); }
|
ATN primary           { $$ = opr(ATN, 1, $2); }
|
SQR primary           { $$ = opr(SQR, 1, $2); }
|
EXP primary           { $$ = opr(EXP, 1, $2); }
|
LN primary            { $$ = opr(LN, 1, $2); }
|
LOG primary           { $$ = opr(LOG, 1, $2); }
|
INT primary           { $$ = opr(INT, 1, $2); }
|
FRAC primary          { $$ = opr(FRAC, 1, $2); }
|
ABS primary           { $$ = opr(ABS, 1, $2); }
|
SGN primary           { $$ = opr(SGN, 1, $2); }
|
RND LPAREN expression COMMA expression RPAREN { $$ = opr(RND, 2, $3, $5); }
|
RAN                      { $$ = opr(RAN, 0); }
|
PI                       { $$ = opr(PI, 0); }
|
LEN LPAREN str_variable RPAREN    { $$ = opr(LEN, 1, $3); }
|
VAL LPAREN str_variable RPAREN    { $$ = opr(VAL, 1, $3); }
;

num_variable:
NAME                          { $$ = id($1, NULL); }
|
NAME LPAREN expression RPAREN { $$ = id($1, $3); }
;

str_variable:
STRVAR                          { $$ = strvar($1, NULL); }
|
STRVAR LPAREN expression RPAREN { $$ = strvar($1, $3); }
;

str_literal:
STRING        { $$ = str($1); }

prog_area:
HASH expression { $$ = opr(HASH, 1, $2); }

%%

int yyerror(char *s)
{
  warning(s, (char *)0);
  return 0;
}

int warning(char *s, char *t)
{
  fprintf(stderr, "%s\n", s);
  if (t)
  fprintf(stderr, " %s\n" , t);
  return 0;
}
