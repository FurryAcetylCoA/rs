//
// Created by AcetylCoA on 2022/11/2.
//

#include "crc16.h"



/**
* @brief calculate crc checksum in command
* stm32自带的crc外设只能算crc32。所以crc16只能用软件算
* @param buf: command buffer pointer
* @param len: length without last 2 bytes of CRC
*/
uint16_t crc16(const uint8_t *buf,uint32_t len){
    uint32_t c, crc=0xFFFF;
    for(int i=0; i<len; i++)
    {
        c = *((uint8_t*)buf +i ) & 0x00FF;
        crc ^= c;
        for(int j=0; j<8; j++)
        {
            if (crc & 0x0001)
            {
                crc >>= 1;
                crc ^= 0xA001;
            }else {
                crc >>= 1;
            }
        }
    }
    return crc;
}