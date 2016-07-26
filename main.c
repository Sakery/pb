
#include <stdio.h>
#include "Device.h"
#include "UI.h"
#include "pb.h"

#include "pb.tab.h"
extern int start_token;

extern FILE *yyin;

Device *device;

int main(int argc, char const *argv[]) {
  UI *ui = UI_create();
  device = Device_create(ui);

  yyin = fopen(argv[1], "r");
  start_token = START_PROGRAM;
  yyparse();
  fclose(yyin);

  Device_run(device, 0);
  /* Device_listAll(device); */

  Device_destroy(device);
  device = NULL;

  UI_destroy(ui);
  ui = NULL;

  return 0;
}
