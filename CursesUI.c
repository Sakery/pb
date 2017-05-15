
#include "UI.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <ncurses.h>

typedef struct UI {
  WINDOW *console_win;
  WINDOW *printer_win;
} UI;

UI *UI_create() {
  UI *self = calloc(1, sizeof(UI));
  initscr();
  // raw();
  cbreak();
  noecho();

  self->console_win = newwin(3, 20, 0, 0);
  wrefresh(self->console_win);

  self->printer_win = newwin(20, 20, 3, 0);
  box(self->printer_win, 0, 0);
  wrefresh(self->printer_win);

  nodelay(self->console_win, TRUE);
  keypad(self->console_win, TRUE);

  return self;
}

void UI_destroy(UI *self) {
  delwin(self->console_win);
  delwin(self->printer_win);
  endwin();
  free(self);
}

void UI_printf(UI *self, const char* format, ...) {
  va_list argptr;
  va_start(argptr, format);
  vwprintw(self->console_win, format, argptr);
  va_end(argptr);
  wrefresh(self->console_win);
}

void UI_gets(UI *self, char *str, int n) {
  int pos = 0;
  int c = 0;
  while (c != 10) {
    c = wgetch(self->console_win);
    if (c != ERR) {

      if (c > KEY_CODE_YES) {
        continue;
      }

      if (c == 10) {
        break;
      } else if (c == 0x7f) {
        if (pos > 0) {
          --pos;
          str[pos] = 0;
        }
        wmove(self->console_win, 0, pos);
        waddch(self->console_win, ' ');
        wmove(self->console_win, 0, pos);
        continue;
      }

      // Invert upper and lower case
      if (islower(c))
        c = toupper(c);
      else if (isupper(c))
        c = tolower(c);

      str[pos] = c;
      str[pos + 1] = 0;

      wmove(self->console_win, 0, pos);
      waddch(self->console_win, c);

      // wmove(self->console_win, 0, 0);
      // wprintw(self->console_win, "%d (%c), %d", c, c, pos);

      if (pos - 1 < n)
        ++pos;
    }
    usleep(100);
  }
}

char UI_key(UI *self) {
  char c = wgetch(self->console_win);
  if (c == ERR)
    return 0;
  return c;
}

void UI_stop(UI *self) {
  char c;
  while (1) {
    c = wgetch(self->console_win);
    if (c != ERR)
      break;
    usleep(100);
  }
  ungetch(c);
}

void UI_csr(UI *self, int x) {
  wmove(self->console_win, 0, x);
}

void UI_clear(UI *self) {
  werase(self->console_win);
}

void UI_ready(UI *self, int mode, int prog_area) {
  char c;
  curs_set(0);
  werase(self->console_win);
  if (mode == 0) {
    /* It is a great pleasure to output... */
    wprintw(self->console_win, "READY P%d", prog_area);
    while (1) {
      c = wgetch(self->console_win);
      if (c != ERR)
        break;
      usleep(100);
    }
  } else {
    int blink = 0;
    int i;
    while (1) {
      char buf[13] = "P ";
      for (i = 0; i < 10; ++i) {
        if (prog_area == i && blink)
          buf[i + 2] = '_';
        else
          buf[i + 2] = '0' + i;
      }
      buf[12] = 0;
      wmove(self->console_win, 0, 0);
      wprintw(self->console_win, buf);
      for (i = 0; i < 5000; ++i) {
        c = wgetch(self->console_win);
        if (c != ERR)
          goto clear;
        usleep(100);
      }
      blink = !blink;
    }
  }
clear:
  ungetch(c);
  werase(self->console_win);
  curs_set(1);
}
