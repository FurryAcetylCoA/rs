//
// Created by 11096 on 2022/11/2.
//

#include <string.h>
#include "rs485.h"


//阻塞式发送+接收
void rs485_send(const uint8_t txbuf[],uint8_t rxbuf[],uint32_t txsize, uint32_t rxsize){
    HAL_StatusTypeDef  ret;
    HAL_GPIO_WritePin(RS485_RE_GPIO_Port,RS485_RE_Pin,GPIO_PIN_SET);
    ret=HAL_UART_Transmit(&huart1, txbuf, txsize, 0xff);
    HAL_GPIO_WritePin(RS485_RE_GPIO_Port,RS485_RE_Pin,GPIO_PIN_RESET);
    if(ret != HAL_OK){
        _TRAP;
    }
    //检错不发生在这层
    uint8_t rxbuf_tmp[16];
    HAL_UART_Receive(&huart1,rxbuf_tmp,rxsize+1,0x1f);
    //接收不能用非阻塞，因为很难界定到底收完了没有
    if(rxbuf != NULL){
        memcpy(rxbuf,rxbuf_tmp,rxsize+1);
    }

}
