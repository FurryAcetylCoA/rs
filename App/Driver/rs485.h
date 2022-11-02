//
// Created by 11096 on 2022/11/2.
//

#ifndef RS_RS485_H
#define RS_RS485_H
#include "main.h"
#include "datastruct.h"


void rs485_send(const uint8_t buf[], uint32_t txsize, uint32_t rxsize);


#endif //RS_RS485_H
