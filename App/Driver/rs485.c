//
// Created by 11096 on 2022/11/2.
//

#include "rs485.h"
#include "crc16.h"

void rs485_send_do(const uint8_t buf[], uint32_t txsize, uint32_t rxsize);
//阻塞式发送
//带重试功能
void rs485_send(const uint8_t buf[], uint32_t txsize, uint32_t rxsize) {
    rs485_send_do(buf,txsize,rxsize);
}
void rs485_send_do(const uint8_t buf[], uint32_t txsize, uint32_t rxsize){
    HAL_StatusTypeDef  ret;
    uint8_t rxbuf[12]={0};
    HAL_GPIO_WritePin(RS485_RE_GPIO_Port,RS485_RE_Pin,GPIO_PIN_SET);
    ret=HAL_UART_Transmit(&huart1, buf, txsize, 0x1ff);
    HAL_GPIO_WritePin(RS485_RE_GPIO_Port,RS485_RE_Pin,GPIO_PIN_RESET);
    if(ret != HAL_OK){
        _TRAP;
    }
    //检错不应该发生在这层

    crc_t crc;

    ret= HAL_UART_Receive(&huart1,rxbuf,rxsize+1,0xff);
    if(ret == HAL_TIMEOUT){
        //先检查是不是“最开头直接就是数据”的情况
        crc.U= crc16(rxbuf,9-2); //在这种情况下 应当能通过CRC检查
        if (crc.Hi == rxbuf[8] && crc.Lo == rxbuf[7]){
            _TRAP;
        }else{
            _TRAP;
        }
    }else{ //收到满长，但任然要判断是不是正却的数据
        crc.U = crc16(rxbuf+1,9-2);
        if (crc.Hi == rxbuf[9] && crc.Lo == rxbuf[8]) {
            _TRAP;
        }else{
            _TRAP;
        }
    }
}