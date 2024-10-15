#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "mdadm.h"
#include "jbod.h"

int mdadm_mount(void) {
  if(jbod_operation(0x00000000000000000000000000000000, NULL) == 0) {
    return 1;
  } else {
    return -1;
  }
}

int mdadm_unmount(void) {
  uint32_t did = 0x0000;
  uint32_t bid = 0x00000000;
  bid = bid << 4;
  uint32_t com = 0x00000001;
  com = com << 12;
  uint32_t res = 0x000000000000;
  res = res << 16;
  uint32_t all = did | bid | com | res;
  if(jbod_operation(all, NULL) == 0) {
    return 1;
  } else {
    return -1;
  }
}

int mdadm_read(uint32_t start_addr, uint32_t read_len, uint8_t *read_buf)  {
  //Complete your code here
  return 0;
}

