//
// Created by AcetylCoA on 2022/11/2.
//

#ifndef RS_CRC16_H
#define RS_CRC16_H
#include "main.h"

typedef __PACKED_UNION{
    uint16_t U;
    __PACKED_STRUCT{
        uint8_t Lo;
        uint8_t Hi;
    };
}crc_t;

uint16_t crc16(const uint8_t *buf,uint32_t len);

#endif //RS_CRC16_H
