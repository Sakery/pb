
#include <stdio.h>
#include <stdlib.h>
#include "Statement.h"
#include "pb.h"

Statement *Statement_create(struct nodeTypeTag *op, Statement *next) {
  Statement *stmnt = calloc(1, sizeof(Statement));
  stmnt->op = op;
  stmnt->next_statement = next;
  return stmnt;
}

void Statement_destroy(Statement *self) {
  free(self);
}

void Statement_dump(Statement *self) {
  if (self->line_num)
    printf("%d ", self->line_num);
  dump(self->op);
  printf("\n");
}

void Statement_dumpLine(Statement *self) {
  if (self->line_num)
    printf("%d ", self->line_num);
  dump(self->op);
  if (self->next_statement && self->next_statement->line_num == 0) {
    printf(":");
    Statement_dumpLine(self->next_statement);
  }
  if (self->line_num)
    printf("\n");
}
