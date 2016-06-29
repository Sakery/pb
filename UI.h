#ifndef UI_H__
#define UI_H__

#include <ncurses.h>

typedef struct UITag {
  WINDOW *console_win;
  WINDOW *printer_win;
} UI;

UI *UI_create();
void UI_destroy(UI *self);
void UI_printf(UI *self, const char* format, ...);
void UI_gets(UI *self, char *str, int n);
void UI_stop(UI *self);
void UI_csr(UI *self, int x);
void UI_clear(UI *self);

#endif /* UI_H__ */
