
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <signal.h>
#include "Device.h"
#include "Statement.h"
#include "SimpleUI.h"
#include "pb.h"
#include "pb.tab.h"

/* extern FILE *yyin; */
extern int start_token;
#ifndef YY_TYPEDEF_YY_BUFFER_STATE
#define YY_TYPEDEF_YY_BUFFER_STATE
typedef struct yy_buffer_state *YY_BUFFER_STATE;
#endif
YY_BUFFER_STATE yy_scan_string (const char *yy_str);
void yy_delete_buffer (YY_BUFFER_STATE b);
void yyrestart  (FILE * input_file);

typedef struct ForLoopTag {
  Statement *loop_statement;
  int var_id;
  double limit;
  double step;
} ForLoop;

static void _push_for_loop(Device *self, Statement *loop_statement, int var_id, double limit, double step);
static void _pop_for_loop(Device *self);
static ForLoop *_for_loop(Device *self);
static int _test_for_loop(Device *self);
static void _step_for_loop(Device *self);
static Statement *_findForLoopExit(Statement *first_statement, int var_id);
static Statement *_findStatement(Statement *first_statement, int line_num);
static Statement *_findNextLine(Statement *statement);
static void _destroyProgram(Statement *first_statement);
static double _ex(Device *self, nodeType *n);

static volatile int keepRunning = 1;

void intHandler(int dummy) {
  keepRunning = 0;
  signal(SIGINT, NULL);
}

Device *Device_create(UI *ui) {
  Device *device = calloc(1, sizeof(Device));
  device->ui = ui;
  device->for_loop_index = -1;
  device->subs_index = -1;
  return device;
}

void Device_destroy(Device *self) {
  int i;
  for (i = 0; i < 10; ++i) {
    _destroyProgram(self->program[i]);
    self->program[i] = NULL;
  }
  while (_for_loop(self))
    _pop_for_loop(self);
  free(self);
}

void Device_loadFile(Device *self, int prog_area, const char *filename) {
  FILE *f = fopen(filename, "r");
  if (f) {
    start_token = START_PROGRAM;
    yyrestart(f);
    yyparse();
    fclose(f);
  }
}

void Device_run(Device *self, int prog_area, int line_num) {

  signal(SIGINT, intHandler);

  UI_clear(self->ui);
  UI_csr(self->ui, 0);

  self->curr_prog_area = prog_area;
  if (line_num) {
    self->curr_statement = _findStatement(self->program[self->curr_prog_area], line_num);
  } else {
    self->curr_statement = self->program[self->curr_prog_area];
  }

  /* TODO Check curr_statement is valid */

  while (self->curr_statement && keepRunning) {
    /* Statement_dump(self->curr_statement, self->ui); */
    Device_executeStatement(self, self->curr_statement);

    struct timespec ts = { 0, 1000000 };
    nanosleep(&ts, NULL);
  }
}

void Device_mainLoop(Device *self) {

  UI_clear(self->ui);
  UI_csr(self->ui, 0);

  char buf[MAX_OUTPUT];

  int last_wrt_mode = -1;

  while (1) {

    if (last_wrt_mode != self->wrt_mode) {
      UI_ready(self->ui, self->wrt_mode, self->curr_prog_area);
    }
    last_wrt_mode = self->wrt_mode;

    UI_gets(self->ui, buf, MAX_OUTPUT);
    if (strlen(buf) == 0)
      continue;

    self->curr_prog_area = 0;
    self->curr_statement = NULL;

    YY_BUFFER_STATE bs = yy_scan_string(buf);
    if (self->wrt_mode == 0)
      start_token = START_STATEMENT;
    else
      start_token = START_WRT_MODE;
    yyparse();
    yy_delete_buffer(bs);

    if (self->curr_statement) {
      Statement *s = self->curr_statement;
      /* Statement_dump(s, self->ui); */
      Device_executeStatement(self, s);
      Statement_destroy(s);
    }
  }
}

void Device_list(Device *self, int prog_area) {
  self->curr_prog_area = prog_area;
  self->curr_statement = self->program[self->curr_prog_area];
  while (self->curr_statement) {
    if (self->curr_statement->line_num)
      Statement_dumpLine(self->curr_statement, self->ui);
    self->curr_statement = self->curr_statement->next_statement;
  }
}

void Device_listAll(Device *self) {
  for (int i = 0; i < 10; ++i) {
    if (self->program[i]) {
      UI_printf(self->ui, "\nP%d\n\n", i);
      Device_list(self, i);
    }
  }
}

void Device_addStatement(Device *self, int prog_area, Statement *stmnt) {
  Statement *first_stmnt = self->program[prog_area];
  if (!first_stmnt) {
    /* printf("adding first line\n"); */
    first_stmnt = stmnt;
  } else {
    Statement *curr = first_stmnt;
    Statement *prev = NULL;
    while (curr) {
      /* have we stepped past the line number of interest? */
      if (stmnt->line_num && curr->line_num && curr->line_num > stmnt->line_num) {
        if (prev) {
          /* insert this line between prev and curr */
          prev->next_statement = stmnt;
          /* printf("inserting line\n"); */
        } else {
          /* otherwise this line becomes the first line */
          first_stmnt = stmnt;
          /* printf("replacing first line\n"); */
        }
        stmnt->next_statement = curr;
        break;
      }
      prev = curr;
      curr = curr->next_statement;
    }
    if (!curr) {
      prev->next_statement = stmnt;
      /* printf("appending to the end\n"); */
    }
  }
  self->program[prog_area] = first_stmnt;
}

static Statement *_findStatement(Statement *first_statement, int line_num) {
  Statement *p = first_statement;
  while (p) {
    if (p->line_num == line_num)
      return p;
    p = p->next_statement;
  }
  return NULL;
}

static Statement *_findNextLine(Statement *statement) {
  Statement *p = statement;
  while (p) {
    p = p->next_statement;
    if (p && p->line_num)
      return p;
  }
  return NULL;
}

static void _destroyProgram(Statement *first_statement) {
  Statement *p = first_statement;
  while (p) {
    Statement *next = p->next_statement;
    Statement_destroy(p);
    p = next;
  }
}

static void info(const char *str) {
  /* printf("%s\n", str); */
}

/*
static void spit(int level, nodeType *n) {
  int i;
  for (i = 0; i < level; ++i)
    printf(" ");
  printf("node: %d", n ? n->type : -1);
  if (n && n->type == typeOpr) {
    printf(", oper: %d\n", n->opr.oper);
    for (i = 0; i < n->opr.nops; ++i)
      spit(level + 2, n->opr.op[i]);
  } else {
    printf("\n");
  }
}
*/

static double _angular_unit_to_rad(Device *self, double d) {
  if (self->angular_unit == ANGULAR_UNIT_DEG)
    return d * 2 * M_PI / 360.0 ;
  else if (self->angular_unit == ANGULAR_UNIT_GRA)
    return d * 2 * M_PI / 400.0;
  else
    return d;
}

static double _rad_to_angular_unit(Device *self, double d) {
  if (self->angular_unit == ANGULAR_UNIT_DEG)
    return d * 360.0 / (2 * M_PI) ;
  else if (self->angular_unit == ANGULAR_UNIT_GRA)
    return d * 400.0 / (2 * M_PI);
  else
    return d;
}

static void _push_char_result(Device *self) {
  ++self->char_result_stack_index;
}

static void _pop_char_result(Device *self) {
  --self->char_result_stack_index;
}

static char *_char_result(Device *self) {
  return self->char_result_stack[self->char_result_stack_index];
}

static void _reset_char_result(Device *self) {
  self->char_result_stack[self->char_result_stack_index][0] = 0;
}

static void _push_for_loop(
  Device *self, Statement *loop_statement, int var_id, double limit, double step) {
  ForLoop *for_loop = malloc(sizeof(ForLoop));
  for_loop->loop_statement = loop_statement;
  for_loop->var_id = var_id;
  for_loop->limit = limit;
  for_loop->step = step;

  ++self->for_loop_index;
  self->for_loops[self->for_loop_index] = for_loop;
}

static void _pop_for_loop(Device *self) {
  free(self->for_loops[self->for_loop_index]);
  self->for_loops[self->for_loop_index] = NULL;
  --self->for_loop_index;
}

static ForLoop *_for_loop(Device *self) {
  int i = self->for_loop_index;
  return i > -1 ? self->for_loops[i] : NULL;
}

static int _test_for_loop(Device *self) {
  ForLoop *loop = _for_loop(self);
  if (loop->step > 0)
    return self->sym[loop->var_id] <= loop->limit;
  else
    return self->sym[loop->var_id] >= loop->limit;
}

static void _step_for_loop(Device *self) {
  ForLoop *loop = _for_loop(self);
  self->sym[loop->var_id] += loop->step;
}

static Statement *_findForLoopExit(Statement *first_statement, int var_id) {
  Statement *p = first_statement;
  while (p) {
    if (p->op->opr.oper == NEXT &&
      p->op->opr.op[0]->type == typeId &&
      p->op->opr.op[0]->id.i == var_id)
      return p->next_statement;
    p = p->next_statement;
  }
  return NULL;
}

static void _pushSubroutine(Device *self, Statement *ret_statement, int prog_area) {
  ++self->subs_index;
  self->subs[self->subs_index] = ret_statement;
  self->subs_prog_area[self->subs_index] = prog_area;
}

static void _popSubroutine(Device *self) {
  self->subs[self->subs_index] = NULL;
  self->subs_prog_area[self->subs_index] = 0;
  --self->subs_index;
}

static Statement *_subroutine(Device *self) {
  int i = self->subs_index;
  return i > -1 ? self->subs[i] : NULL;
}

static int _subroutine_prog_area(Device *self) {
  int i = self->subs_index;
  return i > -1 ? self->subs_prog_area[i] : 0;
}

static void _input_phrase(Device *self, nodeType *n) {
  if (n->opr.op[0]) {
    _reset_char_result(self);
    _ex(self, n->opr.op[0]);
    UI_printf(self->ui, "%s", _char_result(self));
  }
  UI_printf(self->ui, "?");
  _reset_char_result(self);
  UI_gets(self->ui, _char_result(self), MAX_OUTPUT);
  UI_clear(self->ui);
  UI_csr(self->ui, 0);
  char *c = strchr(_char_result(self), '\n');
  if (c)
    *c = 0;
  if (n->opr.op[1]->type == typeId)
    self->sym[n->opr.op[1]->id.i] = atof(_char_result(self));
  else if (n->opr.op[1]->id.i >= 0)
    strncpy(self->str_var[n->opr.op[1]->id.i], _char_result(self), 7);
  else
    strncpy(self->excl_str_var, _char_result(self), 30);
}

static void _input_expression(Device *self, nodeType *n) {
  _input_phrase(self, n->opr.op[0]);
  if (n->opr.nops > 1)
    _input_expression(self, n->opr.op[1]);
}

static void _input(Device *self, nodeType *n) {
  _input_expression(self, n->opr.op[0]);
}

static void _key(Device *self) {
  int c;
  _reset_char_result(self);
  c = UI_key(self->ui);
  if (c != 0) {
    _char_result(self)[0] = c;
    _char_result(self)[1] = 0;
  }
}

static void _print(Device *self, nodeType *char_expr) {
  if (char_expr) {
    _reset_char_result(self);
    _ex(self, char_expr);
    UI_printf(self->ui, _char_result(self));
  } else {
    self->no_cr = 0;
  }
  if (!self->no_cr) {
    UI_stop(self->ui);
    UI_clear(self->ui);
    UI_csr(self->ui, 0);
  }
  self->no_cr = 0;
}

static void _print_csr(Device *self, nodeType *expr, nodeType *char_expr) {
  int x = _ex(self, expr);
  UI_clear(self->ui);
  UI_csr(self->ui, x);
  _print(self, char_expr);
}

static int _jump(Device *self, nodeType *expr) {
  if (expr->type == typeOpr && expr->opr.oper == HASH) {
      int prog_area = _ex(self, expr);
      if (prog_area < 0 || prog_area > 9) {
        UI_printf(self->ui, "Invalid program area\n");
        return 0;
      }
      self->curr_prog_area = prog_area;
      self->curr_statement = self->program[self->curr_prog_area];
      self->curr_statement_modified = 1;
  } else {
    int line_num = _ex(self, expr);
    self->curr_statement = _findStatement(self->program[self->curr_prog_area], line_num);
    self->curr_statement_modified = 1;
    if (!self->curr_statement) {
      UI_printf(self->ui, "Invalid line number\n");
      return 0;
    }
  }
  return 1;
}

static void _goto(Device *self, nodeType *expr) {
  _jump(self, expr);
}

static int _comparison(Device *self, nodeType *comp) {
  double lhs;
  double rhs;
  if (comp->opr.flags == 1) {
    char buf0[31];
    char buf1[31];
    _reset_char_result(self);
    _ex(self, comp->opr.op[0]);
    strcpy(buf0, _char_result(self));
    _reset_char_result(self);
    _ex(self, comp->opr.op[1]);
    strcpy(buf1, _char_result(self));
    lhs = strcmp(buf0, buf1);
    rhs = 0;
    /* UI_printf(self->ui, "** STRING COMPARISON (%G) ** (%s) (%s)\n", lhs, buf0, buf1); */
  } else {
    lhs = _ex(self, comp->opr.op[0]);
    rhs = _ex(self, comp->opr.op[1]);
  }
  switch (comp->opr.oper) {
  case EQ:
    return lhs == rhs;
  case NE:
    return lhs != rhs;
  case LT:
    return lhs < rhs;
  case LE:
    return lhs <= rhs;
  case GT:
    return lhs > rhs;
  case GE:
    return lhs >= rhs;
  }
  return 0;
}

static void _if_semicolon(Device *self, nodeType *comp, nodeType *statement) {
  if (_comparison(self, comp)) {
    _ex(self, statement);
  } else {
    self->curr_statement = _findNextLine(self->curr_statement);
    self->curr_statement_modified = 1;
  }
}

static void _if_then(Device *self, nodeType *comp, nodeType *expr) {
  if (_comparison(self, comp))
    _jump(self, expr);
}

static void _gosub(Device *self, nodeType *expr) {
  Statement *return_to_statement = self->curr_statement->next_statement;
  int prog_area = self->curr_prog_area;
  if (_jump(self, expr))
    _pushSubroutine(self, return_to_statement, prog_area);
}

static void _return(Device *self) {
  self->curr_statement = _subroutine(self);
  self->curr_statement_modified = 1;
  self->curr_prog_area = _subroutine_prog_area(self);
  _popSubroutine(self);
}

static void _for(Device *self, nodeType *num_as, nodeType *to_expr, nodeType *step_expr) {
  int var = _ex(self, num_as);
  double limit = _ex(self, to_expr);
  double step = step_expr ? _ex(self, step_expr) : 1.0;
  _push_for_loop(self, self->curr_statement->next_statement, var, limit, step);
  if (!_test_for_loop(self)) {
    self->curr_statement = _findForLoopExit(self->curr_statement->next_statement, var);
    self->curr_statement_modified = 1;
    _pop_for_loop(self);
  }
}

static void _next(Device *self, nodeType *num_var) {
  /* NOTE: we're ignoring the num_var just at the moment */
  _step_for_loop(self);
  if (_test_for_loop(self)) {
    self->curr_statement = _for_loop(self)->loop_statement;
    self->curr_statement_modified = 1;
  } else {
    _pop_for_loop(self);
  }
}

static void _list(Device *self, nodeType *integer) {
  Device_list(self, 0);
}

static void _lista(Device *self) {
  Device_listAll(self);
}

static void _run(Device *self, nodeType *integer) {
  int line_num = _ex(self, integer);
  Device_run(self, 0, line_num);
}

static void _mode(Device *self, nodeType *integer) {
  int mode = _ex(self, integer);
  switch (mode) {
  case 0:
    self->wrt_mode = 0;
    break;
  case 1:
    self->wrt_mode = 1;
    break;
  case 4:
    self->angular_unit = ANGULAR_UNIT_DEG;
    break;
  case 5:
    self->angular_unit = ANGULAR_UNIT_RAD;
    break;
  case 6:
    self->angular_unit = ANGULAR_UNIT_GRA;
    break;
  case 7:
    self->printer_on = 1;
    break;
  case 8:
    self->printer_on = 0;
    break;
  default:
    /* error */
    break;
  }
}

static double _len(Device *self, nodeType *str_var) {
  _push_char_result(self);
  _reset_char_result(self);
  _ex(self, str_var);
  size_t len = strlen(_char_result(self));
  _pop_char_result(self);
  return len;
}

static void _mid(Device *self, nodeType *m, nodeType *n) {
  int start = _ex(self, m) - 1;
  size_t len = n ? _ex(self, n) : strlen(self->excl_str_var);
  strncat(_char_result(self), self->excl_str_var + start, len);
  /* _char_result(self)[len] = 0; */
}

static double _val(Device *self, nodeType *str_var) {
  _push_char_result(self);
  _reset_char_result(self);
  _ex(self, str_var);
  double val = atof(_char_result(self));
  _pop_char_result(self);
  return val;
}

static void _load(Device *self, nodeType *str_literal) {
  if (str_literal) {
    _reset_char_result(self);
    _ex(self, str_literal);
    Device_loadFile(self, 0, _char_result(self));
  } else {
    UI_printf(self->ui, "Need filename\n");
  }
}

void Device_executeStatement(Device *self, Statement *statement) {
  _ex(self, statement->op);

  /* Advance the statement pointer if this statement didn't modify it */
  if (self->curr_statement && !self->curr_statement_modified)
    self->curr_statement = self->curr_statement->next_statement;
  self->curr_statement_modified = 0;
}

/*
 * Returns a double for numeric values. Appends character results
 * to "_char_result"
 */
static double _ex(Device *self, nodeType *n) {
  if (!n)
    return 0;
  switch (n->type) {
  case typeCon:
    info("Numeric literal");
    return n->con.value;
  case typeStringLiteral:
    info("String literal");
    /* printf(">%s<%lu", output_buf, strlen(output_buf)); */
    strcat(_char_result(self), n->str.value);
    /* printf(">%s<%lu", output_buf, strlen(output_buf)); */
    return 0;
  case typeStringVariable:
  {
    info("String variable");
    int index = 0;
    if (n->id.index)
      index = _ex(self, n->id.index);
    if (n->strvar.i >= 0)
      strcat(_char_result(self), self->str_var[n->strvar.i]);
    else
      strcat(_char_result(self), self->excl_str_var);
    break;
  }
  case typeNumToStr:
  {
    info("Convert number to string");
    char buf[255];
    sprintf(buf, "%G", _ex(self, n->numToStr.op));
    strcat(_char_result(self), buf);
    break;
  }
  case typeId:
  {
    info("Numeric variable");
    int index = 0;
    if (n->id.index)
      index = _ex(self, n->id.index);
    return self->sym[n->id.i + index];
  }
  case typeOpr:
    info("Operator");
    switch (n->opr.oper) {
    case INPUT:
      info("INPUT");
      _input(self, n);
      break;
    case KEY:
      info("KEY");
      _key(self);
      break;
    case PRINT:
      info("PRINT");
      if (n->opr.nops == 0)
        _print(self, NULL);
      else if (n->opr.nops == 1)
        _print(self, n->opr.op[0]);
      else
        _print_csr(self, n->opr.op[0], n->opr.op[1]);
      break;
    case GOTO:
      _goto(self, n->opr.op[0]);
      break;
    case IF:
      info("IF");
      if (n->opr.flags == 1)
        _if_semicolon(self, n->opr.op[0], n->opr.op[1]);
      else
        _if_then(self, n->opr.op[0], n->opr.op[1]);
      break;
    case GOSUB:
      _gosub(self, n->opr.op[0]);
      break;
    case RETURN:
      _return(self);
      break;
    case FOR:
      info("FOR");
      _for(self, n->opr.op[0], n->opr.op[1], n->opr.nops > 2 ? n->opr.op[2] : NULL);
      break;
    case NEXT:
      info("NEXT");
      _next(self, n->opr.op[0]);
      break;
    case STOP:
      info("STOP");
      UI_stop(self->ui);
      UI_clear(self->ui);
      break;
    case END:
      info("END");
      self->curr_statement = NULL;
      self->curr_statement_modified = 1;
      break;
    case VAC:
      info("VAC");
      break;
    case LIST:
      info("LIST");
      _list(self, n->opr.nops > 0 ? n->opr.op[0] : NULL);
      break;
    case LISTA:
      info("LISTA");
      _lista(self);
      break;
    case RUN:
      info("RUN");
      _run(self, n->opr.nops > 0 ? n->opr.op[0] : NULL);
      break;
    case MODE:
      info("MODE");
      _mode(self, n->opr.op[0]);
      break;
    case SETE:
      info("SETE");
      /* dump(n->opr.op[0]); */
      break;
    case SETF:
      info("SETF");
      /* dump(n->opr.op[0]); */
      break;
    case SETN:
      info("SETN");
      break;
    case LEN:
      info("LEN");
      return _len(self, n->opr.op[0]);
    case MID:
      info("MID");
      _mid(self, n->opr.op[0], n->opr.nops > 1 ? n->opr.op[1] : NULL);
      break;
    case VAL:
      info("VAL");
      return _val(self, n->opr.op[0]);
    case SAVE:
      info("SAVE");
      break;
    case LOAD:
      info("LOAD");
      _load(self, n->opr.nops > 0 ? n->opr.op[0] : NULL);
      break;
    case SAVEA:
      info("SAVEA");
      break;
    case LOADA:
      info("LOADA");
      break;
    case PLUS:
      info("PLUS");
      if (n->opr.flags == 1) {
        /* concatenate two strings */
        _ex(self, n->opr.op[0]);
        _ex(self, n->opr.op[1]);
        break;
      } else if (n->opr.nops == 1) {
        return _ex(self, n->opr.op[0]);
      } else {
        return _ex(self, n->opr.op[0]) + _ex(self, n->opr.op[1]);
      }
    case MINUS:
      info("MINUS");
      if (n->opr.nops == 1)
        return -_ex(self, n->opr.op[0]);
      else
        return _ex(self, n->opr.op[0]) - _ex(self, n->opr.op[1]);
    case MUL:
      info("MUL");
      return _ex(self, n->opr.op[0]) * _ex(self, n->opr.op[1]);
    case DIV:
      info("DIV");
      return _ex(self, n->opr.op[0]) / _ex(self, n->opr.op[1]);
    case POW:
      info("POW");
      return pow(_ex(self, n->opr.op[0]), _ex(self, n->opr.op[1]));
    case LPAREN:
      info("LPAREN");
      return _ex(self, n->opr.op[0]);
    case SIN:
      info("SIN");
      return sin(_angular_unit_to_rad(self, _ex(self, n->opr.op[0])));
    case COS:
      info("COS");
      return cos(_angular_unit_to_rad(self, _ex(self, n->opr.op[0])));
    case TAN:
      info("TAN");
      return tan(_angular_unit_to_rad(self, _ex(self, n->opr.op[0])));
    case ASN:
      info("ASN");
      return _rad_to_angular_unit(self, asin(_ex(self, n->opr.op[0])));
    case ACS:
      info("ACS");
      return _rad_to_angular_unit(self, acos(_ex(self, n->opr.op[0])));
    case ATN:
      info("ATN");
      return _rad_to_angular_unit(self, atan(_ex(self, n->opr.op[0])));
    case SQR:
      info("SQR");
      return sqrt(_ex(self, n->opr.op[0]));
    case EXP:
      info("EXP");
      return exp(_ex(self, n->opr.op[0]));
    case LN:
      info("LN");
      return log(_ex(self, n->opr.op[0]));
    case LOG:
      info("LOG");
      return log10(_ex(self, n->opr.op[0]));
    case INT:
      info("INT");
      return (int)_ex(self, n->opr.op[0]);
    case FRAC:
    {
      info("FRAC");
      double d = _ex(self, n->opr.op[0]);
      return d - (int) d;
    }
    case ABS:
      info("ABS");
      return fabs(_ex(self, n->opr.op[0]));
    case SGN:
    {
      info("SGN");
      double d = _ex(self, n->opr.op[0]);
      if (d < 0)
        return -1;
      else if (d > 0)
        return 1;
      else
        return 0;
    }
    case RND:
      info("RND");
      return round(_ex(self, n->opr.op[0]));
    case RAN:
      info("RAN");
      return ((double)rand()) / RAND_MAX;
    case PI:
      info("PI");
      return M_PI;
    case ASSIGN_NUM:
    {
      info("ASSIGN_NUM");
      int index = 0;
      if (n->opr.op[0]->id.index)
        index = _ex(self, n->opr.op[0]->id.index);
      self->sym[n->opr.op[0]->id.i + index] = _ex(self, n->opr.op[1]);
      return n->opr.op[0]->id.i + index;
    }
    case ASSIGN_STR:
    {
      info("ASSIGN_STR");
      int index = 0;
      if (n->opr.op[0]->id.index)
        index = _ex(self, n->opr.op[0]->id.index);
      _reset_char_result(self);
      _ex(self, n->opr.op[1]);
      info(_char_result(self));
      if (n->opr.op[0]->strvar.i >= 0) {
        strcpy(self->str_var[n->opr.op[0]->strvar.i + index], _char_result(self));
        return n->opr.op[0]->id.i + index;
      } else {
        strcpy(self->excl_str_var, _char_result(self));
        return -1;
      }
    }
    case EQ:
      info("EQ");
      self->sym[n->opr.op[0]->id.i] = _ex(self, n->opr.op[1]);
      return n->opr.op[0]->id.i;
    case SEMICOLON:
      info("SEMICOLON");
      _ex(self, n->opr.op[0]);
      if (n->opr.nops > 1)
        _ex(self, n->opr.op[1]);
      else
        self->no_cr = 1;
      break;
    case COMMA:
      info("COMMA");
      _ex(self, n->opr.op[0]);
      strcat(_char_result(self), "\n");
      if (n->opr.nops > 1)
        _ex(self, n->opr.op[1]);
      break;
    case HASH:
      info("HASH");
      return _ex(self, n->opr.op[0]);
    }
    break;
  }
  return 0;
}
