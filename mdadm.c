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
  res = res << 20;
  uint32_t all = did | bid | com | res;
  if(jbod_operation(all, NULL) == 0) {
    return 1;
  } else {
    return -1;
  }
}

int mdadm_read(uint32_t start_addr, uint32_t read_len, uint8_t *read_buf)  {
  if(start_addr > (0x111111111111 >> 20)) {
    return -1;
  }
  if(read_len > 1024) {
    return -2;
  }

  uint32_t did = 0x0000;
  uint32_t bid = 0x00000000;
  bid = bid << 4;
  uint32_t com = 0x00000010;
  com = com << 12;
  uint32_t res = 0x000000000000;
  res = res << 20;
  uint32_t all = did | bid | com | res;

  int x = jbod_operation(all, NULL);
  if(x == -1) {
    return -3;
  }

  return 0;
}

