//
// Created by AcetylCoA on 2022/11/12.
//

#include "w25q128.h"

static uint8_t W25QXX_SPI_ReadWriteByte(uint8_t TxData)
{//spi从器件无法产生时钟。所以mcu就算是收的时候，也得发点东西
    uint8_t RxData = 0X00;
    if(HAL_SPI_TransmitReceive(&hspi1, &TxData, &RxData, 1, 10) != HAL_OK)
    {
        RxData = 0XFF;
    }
    return RxData;
}

void w25q128_read_data(uint32_t addr,uint8_t *pbuf,uint32_t length){

    HAL_GPIO_WritePin(F_CS_GPIO_Port,F_CS_Pin,GPIO_PIN_RESET);

    W25QXX_SPI_ReadWriteByte(0X03);
    W25QXX_SPI_ReadWriteByte((uint8_t)((addr) >> 16));
    W25QXX_SPI_ReadWriteByte((uint8_t)((addr) >> 8 ));
    W25QXX_SPI_ReadWriteByte((uint8_t)(addr));

    for (int i = 0; i < length; ++i) {
        pbuf[i] = W25QXX_SPI_ReadWriteByte(0xff);
    }

    HAL_GPIO_WritePin(F_CS_GPIO_Port,F_CS_Pin,GPIO_PIN_SET);
}

