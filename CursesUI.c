
#include "UI.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ncurses.h>

typedef struct UI {
  WINDOW *console_win;
  WINDOW *printer_win;
} UI;

UI *UI_create() {
  UI *self = calloc(1, sizeof(UI));
  initscr();
  // cbreak();
  // keypad(stdscr, TRUE);

  self->console_win = newwin(3, 20, 0, 0);
  wrefresh(self->console_win);

  self->printer_win = newwin(20, 20, 3, 0);
  box(self->printer_win, 0, 0);
  wrefresh(self->printer_win);

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
  wgetnstr(self->console_win, str, n);
}

char UI_key(UI *self) {
  char c = 0;
  c = getch();
  return c;
}

void UI_stop(UI *self) {
  /* wrefresh(self->console_win); */
  wgetch(self->console_win);
}

void UI_csr(UI *self, int x) {
  wmove(self->console_win, 0, x);
}

void UI_clear(UI *self) {
  werase(self->console_win);
}

void UI_ready(UI *self, int mode, int prog_area) {
  if (mode == 0) {
    /* It is a great pleasure to output... */
    UI_printf(self, "READY P%d", prog_area);
    UI_clear(self);
  } else {
    UI_printf(self, "P 0123456789 %d", prog_area);
    UI_clear(self);
  }
}
