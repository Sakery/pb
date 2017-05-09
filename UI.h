#ifndef UI_H__
#define UI_H__

struct UI;

struct UI *UI_create();
void UI_destroy(struct UI *self);
void UI_printf(struct UI *self, const char* format, ...);
void UI_gets(struct UI *self, char *str, int n);
char UI_key(struct UI *self);
void UI_stop(struct UI *self);
void UI_csr(struct UI *self, int x);
void UI_clear(struct UI *self);
void UI_ready(struct UI *self, int mode, int prog_area);

#endif /* UI_H__ */
