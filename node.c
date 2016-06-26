
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "pb.h"
#include "pb.tab.h"

nodeType *con(double value) {
  nodeType *p;

  if ((p = malloc(sizeof(nodeType))) == NULL)
    yyerror("out of memory");

  p->type = typeCon;
  p->con.value = value;
  return p;
}

nodeType *str(char *value) {
  nodeType *p;

  if ((p = malloc(sizeof(nodeType))) == NULL)
    yyerror("out of memory");

  p->type = typeStringLiteral;
  size_t len = strlen(value);
  p->str.value = malloc(len - 1);
  memcpy(p->str.value, value + 1, len - 2);
  p->str.value[len - 2] = 0;
  return p;
}

nodeType *numToStr(nodeType *numOp) {
    nodeType *p;

    /* allocate node */
    if ((p = malloc(sizeof(nodeType))) == NULL)
        yyerror("out of memory");

    /* copy information */
    p->type = typeNumToStr;
    p->numToStr.op = numOp;

    return p;
}

nodeType *id(int i, nodeType *index) {
    nodeType *p;

    /* allocate node */
    if ((p = malloc(sizeof(nodeType))) == NULL)
        yyerror("out of memory");

    /* copy information */
    p->type = typeId;
    p->id.i = i;
    p->id.index = index;

    return p;
}

nodeType *strvar(int i, nodeType *index) {
    nodeType *p;

    /* allocate node */
    if ((p = malloc(sizeof(nodeType))) == NULL)
        yyerror("out of memory");

    /* copy information */
    p->type = typeStringVariable;
    p->strvar.i = i;
    p->strvar.index = index;

    return p;
}

nodeType *opr(int oper, int nops, ...) {
  va_list ap;
  nodeType *p;
  int i;

  if ((p = malloc(sizeof(nodeType) + (nops - 1) * sizeof(nodeType *))) == NULL)
    yyerror("out of memory");

  p->type = typeOpr;
  p->opr.oper = oper;
  p->opr.flags = 0;
  p->opr.nops = nops;
  va_start(ap, nops);
  for (i = 0; i < nops; ++i)
    p->opr.op[i] = va_arg(ap, struct nodeTypeTag *);
  va_end(ap);
  return p;
}

nodeType *oprFlags(int oper, int flags, int nops, ...) {
  va_list ap;
  nodeType *p;
  int i;

  if ((p = malloc(sizeof(nodeType) + (nops - 1) * sizeof(nodeType *))) == NULL)
    yyerror("out of memory");

  p->type = typeOpr;
  p->opr.oper = oper;
  p->opr.flags = flags;
  p->opr.nops = nops;
  va_start(ap, nops);
  for (i = 0; i < nops; ++i)
    p->opr.op[i] = va_arg(ap, struct nodeTypeTag *);
  va_end(ap);
  return p;
}
