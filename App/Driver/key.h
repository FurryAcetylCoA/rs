//
// Created by 11096 on 2022/10/30.
//

#ifndef RS_KEY_H
#define RS_KEY_H
#include "stm32f4xx_hal.h"
typedef __PACKED_UNION {
    __PACKED_STRUCT{
        uint8_t  key0;
        uint8_t  key1;
        uint8_t  key2;
        uint8_t  key3;
    };
    uint32_t U;
}Key_data;

Key_data key_reader();
#endif //RS_KEY_H
