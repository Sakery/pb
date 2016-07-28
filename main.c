
#include <stdio.h>
#include "Device.h"
#include "UI.h"

Device *device;

int main(int argc, const char *argv[]) {
  UI *ui = UI_create();
  device = Device_create(ui);
  if (argv[1]) {
    Device_loadFile(device, 0, argv[1]);
    Device_run(device, 0);
  } else {
    Device_mainLoop(device);
  }
  Device_destroy(device);
  device = NULL;
  UI_destroy(ui);
  ui = NULL;
  return 0;
}
