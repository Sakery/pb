#ifndef UI_H__
#define UI_H__

#include <stdio.h>

typedef struct UITag {
  FILE *out;
  FILE *in;
} UI;

UI *UI_create();
void UI_destroy(UI *self);
void UI_printf(UI *self, const char* format, ...);
void UI_gets(UI *self, char *str, int n);
char UI_key(UI *self);
void UI_stop(UI *self);
void UI_csr(UI *self, int x);
void UI_clear(UI *self);
void UI_ready(UI *self, int mode, int prog_area);

#endif /* UI_H__ */
