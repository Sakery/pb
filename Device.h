#ifndef DEVICE_H__
#define DEVICE_H__

/* forward declarations */
struct UITag;
struct StatementTag;
struct nodeTypeTag;
struct ForLoopTag;

#define MAX_OUTPUT 255

#define ANGULAR_UNIT_DEG 0
#define ANGULAR_UNIT_RAD 1
#define ANGULAR_UNIT_GRA 2

/*
 * The programmable device
 */
typedef struct DeviceTag {
  struct UITag *ui;
  struct StatementTag *program[10];
  int curr_prog_area;
  struct StatementTag *curr_statement;
  int curr_statement_modified;
  double sym[26];
  char str_var[26][8];
  char excl_str_var[31];
  int no_cr;
  char char_result_stack[8][MAX_OUTPUT];
  char char_result_stack_index;
  int angular_unit;
  int printer_on;
  struct ForLoopTag *for_loops[4];
  int for_loop_index;
  struct StatementTag *subs[8];
  int subs_prog_area[8];
  int subs_index;
} Device;

extern Device *device;

Device *Device_create(struct UITag *ui);
void Device_destroy(Device *self);
void Device_run(Device *self, int prog_area);
void Device_list(Device *self, int prog_area);
void Device_listAll(Device *self);
void Device_addStatement(Device *self, int prog_num, struct StatementTag *stmnt);
void Device_executeStatement(Device *self, struct StatementTag *statement);

#endif /* DEVICE_H__ */
