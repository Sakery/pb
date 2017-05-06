
#include "SimpleUI.h"
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <sys/select.h>
#include <sys/termios.h>

#define STDIN_FILENO 0
#define NB_DISABLE 1
#define NB_ENABLE 1

UI *UI_create() {
  UI *self = calloc(1, sizeof(UI));
  self->out = stdout;
  self->in = stdin;
  return self;
}

void UI_destroy(UI *self) {
  free(self);
}

void UI_printf(UI *self, const char* format, ...) {
  va_list argptr;
  va_start(argptr, format);
  vfprintf(self->out, format, argptr);
  va_end(argptr);
}

void UI_gets(UI *self, char *str, int n) {
  fgets(str, n, self->in);

  /* remove the LR from the string */
  char *c = strchr(str, 10);
  if (c)
    *c = 0;
}

int kbhit() {
  struct timeval tv;
  fd_set fds;
  tv.tv_sec = 0;
  tv.tv_usec = 0;
  FD_ZERO(&fds);
  FD_SET(STDIN_FILENO, &fds);
  select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
  return FD_ISSET(STDIN_FILENO, &fds);
}

void nonblock(int state) {
  struct termios ttystate;
  tcgetattr(STDIN_FILENO, &ttystate);
  if (state == NB_ENABLE) {
    ttystate.c_lflag &= ~(ICANON | ECHO);
    ttystate.c_cc[VMIN] = 1;
  } else if (state == NB_DISABLE) {
    ttystate.c_lflag |= ICANON | ECHO;
  }
  tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
}

char UI_key(UI *self) {
  char c = 0;
  nonblock(NB_ENABLE);
  int i = kbhit();
  if (i != 0) {
    c = fgetc(stdin);
  }
  nonblock(NB_DISABLE);
  return c;
}

void UI_stop(UI *self) {
  /* fprintf(self->out, "\n"); */
}

void UI_csr(UI *self, int x) {
  int i;
  for (i = 0; i < x; ++i)
    fprintf(self->out, " ");
}

void UI_clear(UI *self) {
  fprintf(self->out, "\n");
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
