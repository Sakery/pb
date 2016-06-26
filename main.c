
#include <stdio.h>
#include "Device.h"
#include "pb.h"

extern FILE *yyin;

Device *device;

int main(int argc, char const *argv[]) {

  device = Device_create();

  yyin = fopen(argv[1], "r");
  yyparse();
  fclose(yyin);

  Device_run(device, 0);
  /* Device_listAll(device); */
  Device_destroy(device);
  device = NULL;

  return 0;
}
