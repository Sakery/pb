
#include "UI.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/termios.h>

typedef struct UI {
  FILE *out;
  FILE *in;
} UI;

static int kbhit() {
  struct timeval tv;
  fd_set fds;
  tv.tv_sec = 0;
  tv.tv_usec = 0;
  FD_ZERO(&fds);
  FD_SET(STDIN_FILENO, &fds);
  select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
  return FD_ISSET(STDIN_FILENO, &fds);
}

UI *UI_create() {
  UI *self = calloc(1, sizeof(UI));
  self->out = stdout;
  self->in = stdin;

  struct termios ttystate;
  tcgetattr(STDIN_FILENO, &ttystate);
  ttystate.c_lflag &= ~(ICANON | ECHO);
  ttystate.c_cc[VMIN] = 1;
  tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);

  return self;
}

void UI_destroy(UI *self) {
  struct termios ttystate;
  tcgetattr(STDIN_FILENO, &ttystate);
  ttystate.c_lflag |= ICANON | ECHO;
  tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);

  free(self);
}

void UI_printf(UI *self, const char* format, ...) {
  va_list argptr;
  va_start(argptr, format);
  vfprintf(self->out, format, argptr);
  va_end(argptr);
}

void UI_gets(UI *self, char *str, int n) {
  char *ptr = str;
  char c = 0;
  while (c != 10) {
    int i = kbhit();
    if (i != 0) {
      c = fgetc(self->in);
      if (c == 10)
        break;
      ptr[0] = c;
      ptr[1] = 0;
      if (ptr - str - 1 < n)
        ++ptr;
      fprintf(self->out, "%c", c);
      fflush(self->out);
    }
    usleep(100);
  }
}

char UI_key(UI *self) {
  char c = 0;
  int i = kbhit();
  if (i != 0) {
    c = fgetc(stdin);
  }
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
