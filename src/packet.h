#ifndef PKT_STRUCT_H
#define PKT_STRUCT_H

#include "stdint.h"

struct data_packet_t{
    uint32_t head;
    uint32_t sensor1;
    uint32_t sensor2;
    uint32_t crc;
};

#endif