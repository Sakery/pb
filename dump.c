
#include "Statement.h"
#include "UI.h"
#include "pb.h"
#include "pb.tab.h"

void dump(UI *ui, nodeType *n) {
  if (!n)
    return;
  switch (n->type) {
  case typeCon:
    UI_printf(ui, "%G", n->con.value);
    break;
  case typeStringLiteral:
    UI_printf(ui, "\"%s\"", n->str.value);
    break;
  case typeStringVariable:
    if (n->strvar.i >= 0) {
      UI_printf(ui, "%c$", n->strvar.i + 'A');
      if (n->strvar.index) {
        UI_printf(ui, "(");
        dump(ui, n->id.index);
        UI_printf(ui, ")");
      }
    } else {
      UI_printf(ui, "$");
    }
    break;
  case typeNumToStr:
    dump(ui, n->numToStr.op);
    break;
  case typeId:
    UI_printf(ui, "%c", n->id.i + 'A');
    if (n->id.index) {
      UI_printf(ui, "(");
      dump(ui, n->id.index);
      UI_printf(ui, ")");
    }
    break;
  case typeOpr:
    switch (n->opr.oper) {
    case INPUT:
      UI_printf(ui, "INPUT ");
      dump(ui, n->opr.op[0]);
      break;
    case KEY:
      UI_printf(ui, "KEY");
      break;
    case PRINT:
      UI_printf(ui, "PRINT");
      if (n->opr.nops == 1) {        /* character_expression */
        UI_printf(ui, " ");
        dump(ui, n->opr.op[0]);
      } else if (n->opr.nops == 2) { /* CSR expression SEMICOLON character_expression */
        UI_printf(ui, " CSR ");
        dump(ui, n->opr.op[0]);
        UI_printf(ui, ";");
        dump(ui, n->opr.op[1]);
      }
      break;
    case IF:
      UI_printf(ui, "IF ");
      dump(ui, n->opr.op[0]);
      if (n->opr.flags == 1)
        UI_printf(ui, ";");
      else
        UI_printf(ui, " THEN ");
      dump(ui, n->opr.op[1]);
      break;
    case FOR:
      UI_printf(ui, "FOR ");
      dump(ui, n->opr.op[0]);
      UI_printf(ui, " TO ");
      dump(ui, n->opr.op[1]);
      if (n->opr.nops > 2) {
        UI_printf(ui, " STEP ");
        dump(ui, n->opr.op[2]);
      }
      break;
    case NEXT:
      UI_printf(ui, "NEXT ");
      dump(ui, n->opr.op[0]);
      break;
    case STOP:
      UI_printf(ui, "STOP");
      break;
    case END:
      UI_printf(ui, "END");
      break;
    case VAC:
      UI_printf(ui, "VAC");
      break;
    case RUN:
      UI_printf(ui, "RUN");
      if (n->opr.nops > 0) {
        UI_printf(ui, " ");
        dump(ui, n->opr.op[0]);
      }
      break;
    case MODE:
      UI_printf(ui, "MODE ");
      dump(ui, n->opr.op[0]);
      break;
    case SETE:
      UI_printf(ui, "SET E ");
      dump(ui, n->opr.op[0]);
      break;
    case SETF:
      UI_printf(ui, "SET F ");
      dump(ui, n->opr.op[0]);
      break;
    case SETN:
      UI_printf(ui, "SET N");
      break;
    case LEN:
      UI_printf(ui, "LEN(");
      dump(ui, n->opr.op[0]);
      UI_printf(ui, ")");
      break;
    case MID:
      UI_printf(ui, "MID(");
      dump(ui, n->opr.op[0]);
      if (n->opr.nops > 1) {
        UI_printf(ui, ",");
        dump(ui, n->opr.op[1]);
      }
      UI_printf(ui, ")");
      break;
    case VAL:
      UI_printf(ui, "VAL(");
      dump(ui, n->opr.op[0]);
      UI_printf(ui, ")");
      break;
    case SAVE:
      UI_printf(ui, "SAVE");
      if (n->opr.nops == 1) {
        UI_printf(ui, " ");
        dump(ui, n->opr.op[0]);
      }
      break;
    case LOAD:
      UI_printf(ui, "LOAD");
      if (n->opr.nops == 1) {
        UI_printf(ui, " ");
        dump(ui, n->opr.op[0]);
      }
      break;
    case SAVEA:
      UI_printf(ui, "SAVE A");
      if (n->opr.nops == 1) {
        UI_printf(ui, " ");
        dump(ui, n->opr.op[0]);
      }
      break;
    case LOADA:
      UI_printf(ui, "LOAD A");
      if (n->opr.nops == 1) {
        UI_printf(ui, " ");
        dump(ui, n->opr.op[0]);
      }
      break;
    case GOTO:
      UI_printf(ui, "GOTO ");
      dump(ui, n->opr.op[0]);
      break;
    case GOSUB:
      UI_printf(ui, "GOSUB ");
      dump(ui, n->opr.op[0]);
      break;
    case RETURN:
      UI_printf(ui, "RETURN");
      break;
    case PLUS:
      if (n->opr.nops == 1) {
        UI_printf(ui, "+");
        dump(ui, n->opr.op[0]);
      } else {
        dump(ui, n->opr.op[0]);
        UI_printf(ui, "+");
        dump(ui, n->opr.op[1]);
      }
      break;
    case MINUS:
      if (n->opr.nops == 1) {
        UI_printf(ui, "-");
        dump(ui, n->opr.op[0]);
      } else {
        dump(ui, n->opr.op[0]);
        UI_printf(ui, "-");
        dump(ui, n->opr.op[1]);
      }
      break;
    case MUL:
      dump(ui, n->opr.op[0]);
      UI_printf(ui, "*");
      dump(ui, n->opr.op[1]);
      break;
    case DIV:
      dump(ui, n->opr.op[0]);
      UI_printf(ui, "/");
      dump(ui, n->opr.op[1]);
      break;
    case POW:
      dump(ui, n->opr.op[0]);
      UI_printf(ui, "↑");
      dump(ui, n->opr.op[1]);
      break;
    case LPAREN:
      UI_printf(ui, "(");
      dump(ui, n->opr.op[0]);
      UI_printf(ui, ")");
      break;
    case SIN:
      UI_printf(ui, "SIN ");
      dump(ui, n->opr.op[0]);
      break;
    case COS:
      UI_printf(ui, "COS ");
      dump(ui, n->opr.op[0]);
      break;
    case TAN:
      UI_printf(ui, "TAN ");
      dump(ui, n->opr.op[0]);
      break;
    case ASN:
      UI_printf(ui, "ASN ");
      dump(ui, n->opr.op[0]);
      break;
    case ACS:
      UI_printf(ui, "ACS ");
      dump(ui, n->opr.op[0]);
      break;
    case ATN:
      UI_printf(ui, "ATN ");
      dump(ui, n->opr.op[0]);
      break;
    case SQR:
      UI_printf(ui, "SQR ");
      dump(ui, n->opr.op[0]);
      break;
    case EXP:
      UI_printf(ui, "EXP ");
      dump(ui, n->opr.op[0]);
      break;
    case LN:
      UI_printf(ui, "LN ");
      dump(ui, n->opr.op[0]);
      break;
    case LOG:
      UI_printf(ui, "LOG ");
      dump(ui, n->opr.op[0]);
      break;
    case INT:
      UI_printf(ui, "INT ");
      dump(ui, n->opr.op[0]);
      break;
    case FRAC:
      UI_printf(ui, "FRAC ");
      dump(ui, n->opr.op[0]);
      break;
    case ABS:
      UI_printf(ui, "ABS ");
      dump(ui, n->opr.op[0]);
      break;
    case SGN:
      UI_printf(ui, "SGN ");
      dump(ui, n->opr.op[0]);
      break;
    case RND:
      UI_printf(ui, "RND(");
      dump(ui, n->opr.op[0]);
      UI_printf(ui, ",");
      dump(ui, n->opr.op[1]);
      UI_printf(ui, ")");
      break;
    case RAN:
      UI_printf(ui, "RAN#");
      break;
    case PI:
      UI_printf(ui, "π");
      break;
    case ASSIGN_NUM:
    case ASSIGN_STR:
    case EQ:
      dump(ui, n->opr.op[0]);
      UI_printf(ui, "=");
      dump(ui, n->opr.op[1]);
      break;
    case NE:
      dump(ui, n->opr.op[0]);
      UI_printf(ui, "≠");
      dump(ui, n->opr.op[1]);
      break;
    case LT:
      dump(ui, n->opr.op[0]);
      UI_printf(ui, "<");
      dump(ui, n->opr.op[1]);
      break;
    case LE:
      dump(ui, n->opr.op[0]);
      UI_printf(ui, "≤");
      dump(ui, n->opr.op[1]);
      break;
    case GT:
      dump(ui, n->opr.op[0]);
      UI_printf(ui, ">");
      dump(ui, n->opr.op[1]);
      break;
    case GE:
      dump(ui, n->opr.op[0]);
      UI_printf(ui, "≥");
      dump(ui, n->opr.op[1]);
      break;
    case COMMA:
      if (n->opr.op[0]) {
        /* The first part may be NULL if this is an input_phrase without a
           text prompt */
        dump(ui, n->opr.op[0]);
      }
      if (n->opr.nops > 1) {
        if (n->opr.op[0] && n->opr.op[1])
          UI_printf(ui, ",");
        dump(ui, n->opr.op[1]);
      }
      break;
    case SEMICOLON:
      dump(ui, n->opr.op[0]);
      UI_printf(ui, ";");
      if (n->opr.nops > 1)
        dump(ui, n->opr.op[1]);
      break;
    case HASH:
      UI_printf(ui, "#");
      dump(ui, n->opr.op[0]);
      break;
    default:
      UI_printf(ui, "(UNKNOWN)");
      break;
    }
    break;
  }
}
