#ifndef STATEMENT_H__
#define STATEMENT_H__

/* forward declarations */
struct UI;
struct nodeTypeTag;

/*
 * An individual program statement
 */
typedef struct StatementTag {
  int line_num;
  struct nodeTypeTag *op;
  struct StatementTag *next_statement;
} Statement;

Statement *Statement_create(struct nodeTypeTag *op, Statement *next);
void Statement_destroy(Statement *self);
void Statement_dump(Statement *self, struct UI *ui);
void Statement_dumpLine(Statement *self, struct UI *ui);

#endif /* STATEMENT_H__ */
