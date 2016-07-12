
#include "SimpleUI.h"
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

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
}

void UI_stop(UI *self) {
  /* fgetc(self->in); */
}

void UI_csr(UI *self, int x) {
  int i;
  for (i = 0; i < x; ++i)
    fprintf(self->out, " ");
}

void UI_clear(UI *self) {
  fprintf(self->out, "\n");
}
