#ifndef CRC_INIT_H
#define CRC_INIT_H
#include "common_defs.h"


void crc_init(void);
uint32_t crc32_get(uint8_t *bfr, uint32_t len);

#endif 

