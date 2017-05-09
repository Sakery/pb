#ifndef PB_H__
#define PB_H__

struct UI;

typedef enum {
  typeCon,
  typeStringLiteral,
  typeStringVariable,
  typeNumToStr,
  typeId,
  typeOpr
} nodeEnum;

/* constants */
typedef struct {
  double value;
} conNodeType;

/* string literals */
typedef struct {
  char *value;
} stringLiteralNodeType;

/* string variable */
typedef struct {
  int i;
  struct nodeTypeTag *index;
} stringVariableNodeType;

/* number to string */
typedef struct {
  struct nodeTypeTag *op;
} numToStrNodeType;

/* identifiers */
typedef struct {
  int i;
  struct nodeTypeTag *index;
} idNodeType;

/* operators */
typedef struct {
  int oper;
  int flags;
  int nops;
  struct nodeTypeTag *op[1];
} oprNodeType;

typedef struct nodeTypeTag {
  nodeEnum type;
  union {
    conNodeType con;
    stringLiteralNodeType str;
    stringVariableNodeType strvar;
    numToStrNodeType numToStr;
    idNodeType id;
    oprNodeType opr;
  };
} nodeType;

nodeType *con(double value);
nodeType *str(char *value);
nodeType *numToStr(nodeType *numOp);
nodeType *id(int i, nodeType *index);
nodeType *strvar(int i, nodeType *index);
nodeType *opr(int oper, int nops, ...);
nodeType *oprFlags(int oper, int flags, int nops, ...);
void freeNode(nodeType *node);

void dump(struct UI *ui, nodeType *n);
/*
double ex(nodeType *n);

void stash_term_settings();
void restore_term_settings();
void non_blocking_term();
void blocking_term();
*/
int yyparse();
int yylex();
int yyerror(char *s);
int warning(char *s, char *t);

#endif /* PB_H__ */
