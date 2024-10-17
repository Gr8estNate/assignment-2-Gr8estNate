#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "mdadm.h"
#include "jbod.h"

static int ismount = 0;

int mdadm_mount(void) {
  if(jbod_operation(0x00000000000000000000000000000000, NULL) == 0) {
    ismount = 1;
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
    ismount = 0;
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

  if(ismount == 0) {
    return -3;
  }

  uint32_t did = (start_addr / 256.0 / 256.0);
  uint32_t bid = (start_addr / 256.0 / (did + 1.0));

  bid = bid << 4;
  uint32_t com = 2;
  com = com << 12;
  uint32_t res = 0x000000000000;
  res = res << 20;
  uint32_t all = did | bid | com | res;

  uint8_t fullblock0[256];

  jbod_operation(all, fullblock0);

  com = 3;
  com = com << 12;
  all = did | bid | com | res;

  jbod_operation(all, fullblock0);

  com = 4;
  com = com << 12;
  all = did | bid | com | res;

  if(bid == (255 << 4)) {
    jbod_operation(all, fullblock0);
    did += 1;
    bid = 0;
    bid = bid << 4;
    com = 2;
    com = com << 12;
    all = did | bid | com | res;
    jbod_operation(all, fullblock0);
    com = 3;
    com = com << 12;
    all = did | bid | com | res;
    jbod_operation(all, fullblock0);
    com = 4;
    com = com << 12;
    all = did | bid | com | res;
  } else {
    jbod_operation(all, fullblock0);
  }

  uint8_t fullblock1[256];
  if(bid == (255 << 4)) {
    jbod_operation(all, fullblock1);
    did += 1;
    bid = 0;
    bid = bid << 4;
    com = 2;
    com = com << 12;
    all = did | bid | com | res;
    jbod_operation(all, fullblock1);
    com = 3;
    com = com << 12;
    all = did | bid | com | res;
    jbod_operation(all, fullblock1);
    com = 4;
    com = com << 12;
    all = did | bid | com | res;
  } else {
    jbod_operation(all, fullblock1);
  }

  uint8_t allblock[1280];
  for(int n = 0; n < 256; n++) {
    allblock[n] = fullblock0[n];
  }
  for(int p = 0; p < 256; p++) {
    allblock[p + 256] = fullblock1[p];
  }

  if(read_len > 256) {
    uint8_t fullblock2[256];
    if(bid == (255 << 4)) {
      jbod_operation(all, fullblock2);
      did += 1;
      bid = 0;
      bid = bid << 4;
      com = 2;
      com = com << 12;
      all = did | bid | com | res;
      jbod_operation(all, fullblock2);
      com = 3;
      com = com << 12;
      all = did | bid | com | res;
      jbod_operation(all, fullblock2);
      com = 4;
      com = com << 12;
      all = did | bid | com | res;
    } else {
      jbod_operation(all, fullblock2);
    }
    for(int n = 0; n < 256; n++) {
    allblock[n + 512] = fullblock2[n];
    }
  }
  if(read_len > 512) {
    uint8_t fullblock3[256];
    if(bid == (255 << 4)) {
      jbod_operation(all, fullblock3);
      did += 1;
      bid = 0;
      bid = bid << 4;
      com = 2;
      com = com << 12;
      all = did | bid | com | res;
      jbod_operation(all, fullblock3);
      com = 3;
      com = com << 12;
      all = did | bid | com | res;
      jbod_operation(all, fullblock3);
      com = 4;
      com = com << 12;
      all = did | bid | com | res;
    } else {
      jbod_operation(all, fullblock3);
    }
    for(int n = 0; n < 256; n++) {
    allblock[n + 768] = fullblock3[n];
    }
  }
  if(read_len > 768) {
    uint8_t fullblock4[256];
    if(bid == (255 << 4)) {
      jbod_operation(all, fullblock4);
      did += 1;
      bid = 0;
      bid = bid << 4;
      com = 2;
      com = com << 12;
      all = did | bid | com | res;
      jbod_operation(all, fullblock4);
      com = 3;
      com = com << 12;
      all = did | bid | com | res;
      jbod_operation(all, fullblock4);
      com = 4;
      com = com << 12;
      all = did | bid | com | res;
    } else {
      jbod_operation(all, fullblock4);
    }
    for(int n = 0; n < 256; n++) {
    allblock[n + 1024] = fullblock4[n];
    }
  }

  memcpy(read_buf, (allblock + (start_addr % 256)), read_len);
  
  return read_len;
}

