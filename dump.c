
#include <stdio.h>
#include "Statement.h"
#include "pb.h"
#include "pb.tab.h"

void dump(nodeType *n) {
  if (!n)
    return;
  switch (n->type) {
  case typeCon:
    printf("%G", n->con.value);
    break;
  case typeStringLiteral:
    printf("\"%s\"", n->str.value);
    break;
  case typeStringVariable:
    if (n->strvar.i >= 0) {
      printf("%c$", n->strvar.i + 'A');
      if (n->strvar.index) {
        printf("(");
        dump(n->id.index);
        printf(")");
      }
    } else {
      printf("$");
    }
    break;
  case typeNumToStr:
    dump(n->numToStr.op);
    break;
  case typeId:
    printf("%c", n->id.i + 'A');
    if (n->id.index) {
      printf("(");
      dump(n->id.index);
      printf(")");
    }
    break;
  case typeOpr:
    switch (n->opr.oper) {
    case INPUT:
      printf("INPUT ");
      dump(n->opr.op[0]);
      break;
    case KEY:
      printf("KEY");
      break;
    case PRINT:
      printf("PRINT");
      if (n->opr.nops == 1) {        /* character_expression */
        printf(" ");
        dump(n->opr.op[0]);
      } else if (n->opr.nops == 2) { /* CSR expression SEMICOLON character_expression */
        printf(" CSR ");
        dump(n->opr.op[0]);
        printf(";");
        dump(n->opr.op[1]);
      }
      break;
    case IF:
      printf("IF ");
      dump(n->opr.op[0]);
      if (n->opr.flags == 1)
        printf(";");
      else
        printf(" THEN ");
      dump(n->opr.op[1]);
      break;
    case FOR:
      printf("FOR ");
      dump(n->opr.op[0]);
      printf(" TO ");
      dump(n->opr.op[1]);
      if (n->opr.nops > 2) {
        printf(" STEP ");
        dump(n->opr.op[2]);
      }
      break;
    case NEXT:
      printf("NEXT ");
      dump(n->opr.op[0]);
      break;
    case STOP:
      printf("STOP");
      break;
    case END:
      printf("END");
      break;
    case VAC:
      printf("VAC");
      break;
    case MODE:
      printf("MODE ");
      dump(n->opr.op[0]);
      break;
    case SETE:
      printf("SET E ");
      dump(n->opr.op[0]);
      break;
    case SETF:
      printf("SET F ");
      dump(n->opr.op[0]);
      break;
    case SETN:
      printf("SET N");
      break;
    case LEN:
      printf("LEN(");
      dump(n->opr.op[0]);
      printf(")");
      break;
    case MID:
      printf("MID(");
      dump(n->opr.op[0]);
      if (n->opr.nops > 1) {
        printf(",");
        dump(n->opr.op[1]);
      }
      printf(")");
      break;
    case VAL:
      printf("VAL(");
      dump(n->opr.op[0]);
      printf(")");
      break;
    case GOTO:
      printf("GOTO ");
      dump(n->opr.op[0]);
      break;
    case GOSUB:
      printf("GOSUB ");
      dump(n->opr.op[0]);
      break;
    case RETURN:
      printf("RETURN");
      break;
    case PLUS:
      if (n->opr.nops == 1) {
        printf("+");
        dump(n->opr.op[0]);
      } else {
        dump(n->opr.op[0]);
        printf("+");
        dump(n->opr.op[1]);
      }
      break;
    case MINUS:
      if (n->opr.nops == 1) {
        printf("-");
        dump(n->opr.op[0]);
      } else {
        dump(n->opr.op[0]);
        printf("-");
        dump(n->opr.op[1]);
      }
      break;
    case MUL:
      dump(n->opr.op[0]);
      printf("*");
      dump(n->opr.op[1]);
      break;
    case DIV:
      dump(n->opr.op[0]);
      printf("/");
      dump(n->opr.op[1]);
      break;
    case POW:
      dump(n->opr.op[0]);
      printf("↑");
      dump(n->opr.op[1]);
      break;
    case LPAREN:
      printf("(");
      dump(n->opr.op[0]);
      printf(")");
      break;
    case SIN:
      printf("SIN ");
      dump(n->opr.op[0]);
      break;
    case COS:
      printf("COS ");
      dump(n->opr.op[0]);
      break;
    case TAN:
      printf("TAN ");
      dump(n->opr.op[0]);
      break;
    case ASN:
      printf("ASN ");
      dump(n->opr.op[0]);
      break;
    case ACS:
      printf("ACS ");
      dump(n->opr.op[0]);
      break;
    case ATN:
      printf("ATN ");
      dump(n->opr.op[0]);
      break;
    case SQR:
      printf("SQR ");
      dump(n->opr.op[0]);
      break;
    case EXP:
      printf("EXP ");
      dump(n->opr.op[0]);
      break;
    case LN:
      printf("LN ");
      dump(n->opr.op[0]);
      break;
    case LOG:
      printf("LOG ");
      dump(n->opr.op[0]);
      break;
    case INT:
      printf("INT ");
      dump(n->opr.op[0]);
      break;
    case FRAC:
      printf("FRAC ");
      dump(n->opr.op[0]);
      break;
    case ABS:
      printf("ABS ");
      dump(n->opr.op[0]);
      break;
    case SGN:
      printf("SGN ");
      dump(n->opr.op[0]);
      break;
    case RND:
      printf("RND(");
      dump(n->opr.op[0]);
      printf(",");
      dump(n->opr.op[1]);
      printf(")");
      break;
    case RAN:
      printf("RAN#");
      break;
    case PI:
      printf("π");
      break;
    case ASSIGN_NUM:
    case ASSIGN_STR:
    case EQ:
      dump(n->opr.op[0]);
      printf("=");
      dump(n->opr.op[1]);
      break;
    case NE:
      dump(n->opr.op[0]);
      printf("≠");
      dump(n->opr.op[1]);
      break;
    case LT:
      dump(n->opr.op[0]);
      printf("<");
      dump(n->opr.op[1]);
      break;
    case LE:
      dump(n->opr.op[0]);
      printf("≤");
      dump(n->opr.op[1]);
      break;
    case GT:
      dump(n->opr.op[0]);
      printf(">");
      dump(n->opr.op[1]);
      break;
    case GE:
      dump(n->opr.op[0]);
      printf("≥");
      dump(n->opr.op[1]);
      break;
    case COMMA:
      if (n->opr.op[0]) {
        /* The first part may be NULL if this is an input_phrase without a
           text prompt */
        dump(n->opr.op[0]);
      }
      if (n->opr.nops > 1) {
        if (n->opr.op[0] && n->opr.op[1])
          printf(",");
        dump(n->opr.op[1]);
      }
      break;
    case SEMICOLON:
      dump(n->opr.op[0]);
      printf(";");
      if (n->opr.nops > 1)
        dump(n->opr.op[1]);
      break;
    case HASH:
      printf("#");
      dump(n->opr.op[0]);
      break;
    default:
      printf("(UNKNOWN)");
      break;
    }
    break;
  }
}
