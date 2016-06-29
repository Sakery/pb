
#include <stdio.h>
#include <stdlib.h>
#include "Statement.h"
#include "UI.h"
#include "pb.h"

Statement *Statement_create(struct nodeTypeTag *op, Statement *next) {
  Statement *self = calloc(1, sizeof(Statement));
  self->op = op;
  self->next_statement = next;
  return self;
}

void Statement_destroy(Statement *self) {
  freeNode(self->op);
  free(self);
}

void Statement_dump(Statement *self, UI *ui) {
  if (self->line_num)
    UI_printf(ui, "%d ", self->line_num);
  dump(ui, self->op);
  UI_printf(ui, "\n");
}

void Statement_dumpLine(Statement *self, UI *ui) {
  if (self->line_num)
    UI_printf(ui, "%d ", self->line_num);
  dump(ui, self->op);
  if (self->next_statement && self->next_statement->line_num == 0) {
    UI_printf(ui, ":");
    Statement_dumpLine(self->next_statement, ui);
  }
  if (self->line_num)
    UI_printf(ui, "\n");
}
