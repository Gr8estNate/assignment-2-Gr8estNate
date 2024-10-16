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
  if((start_addr + read_len) > 1048576) {
    return -1;
  }
  if(read_len > 1024) {
    return -2;
  }

  if((read_len != 0) && (read_buf == NULL)) {
    return -4;
  }

  uint32_t p = (0x00000000000000000000 << 12);

  if(jbod_operation(p, NULL) == 0) {
    p = (0x00000000000000000001 << 12);
    jbod_operation(p, NULL);
    return -3;
  }

  int calc_did = start_addr / 256.0 / 256.0;
  int calc_bid = start_addr / 256.0 / calc_did;
  //int calc_truestart = start_addr % 256;

  uint32_t did = 0x0000;
  uint32_t bid = 0x00000000;

  for(int i = 0; i < 4; i++) {
    if((calc_did % 2) == 1) {
      did = did | (1 << i);
    }
    calc_did = calc_did / 2;
  }

  /*
  for(int j = 0; j <= 4; j++){
    printf(" clajdilk : %d", (did >> (4 - j)) & 1);
  }
  */

  for(int i = 0; i < 8; i++) {
    if((calc_bid % 2) == 1) {
      bid = bid | (1 << i);
    }
    calc_bid = calc_bid / 2;
  }

  bid = bid << 4;
  uint32_t com = 0x00000010;
  com = com << 12;
  uint32_t res = 0x000000000000;
  res = res << 20;
  uint32_t all = did | bid | com | res;

  jbod_operation(all, NULL);

  com = (0x00000011 << 12);
  all = did | bid | com | res;

  jbod_operation(all, NULL);

  uint8_t fullblock[1024];
  com = (0x00000100 << 12);
  all = did | bid | com | res;

  jbod_operation(all, fullblock);

  for(int k = 0; k < read_len; k++) {
    *(read_buf + k) = fullblock[k];
  }

  return read_len;
}

