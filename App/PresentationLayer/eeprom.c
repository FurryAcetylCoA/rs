//
// Created by 11096 on 2022/10/27.
//

#include "eeprom.h"
#include "string.h"
#include "stdio.h"

static uint32_t EE_checked;

static uint16_t miniPow(uint8_t  f);
static uint8_t  miniLog(uint16_t f);


// 初始化检查EEPROM是否可以读取
HAL_StatusTypeDef EE_init(){
    HAL_StatusTypeDef  hr;
    uint8_t buffer;
    //测试EEPROM是否可以正常读写，防止待会读出脏数据
    uint8_t string[]={"CoA"__TIME__};
    uint8_t string_echo[sizeof(string)];

    EE_write(231,string,sizeof(string));

    HAL_Delay(1);
    EE_read (231,string_echo,sizeof(string));
    if(hr != HAL_OK){
        _TRAP;
        return hr;
    }
    hr = (0 == strcmp((char*)string, (char*)string_echo)) ? HAL_OK : HAL_ERROR;

    if (hr != HAL_OK){
        _TRAP;
        return hr;
    }
    //测试EEPROM内的数据有没有初始化过
    EE_read(230,&buffer,1);

    if(buffer != 0x32) {
        E2PRom_data zero_data;
        memset(&zero_data, 0, sizeof(E2PRom_data));
        EE_write(0, (uint8_t *) &zero_data, sizeof(E2PRom_data));
        buffer = 0x32;
        EE_write(230, &buffer, 1);
    }
    EE_checked=1;
    return HAL_OK;
}


HAL_StatusTypeDef EE_Load(App_info *appInfo){
    E2PRom_data  ERom;
    uint16_t     tempFactor;
    if(EE_checked != 1){
        HAL_StatusTypeDef ret;
        ret = EE_init();
        if(ret != HAL_OK){
            EE_checked = 4;
            _TRAP;
            return ret;
        }
    }
    EE_read(0x00, (uint8_t*)&ERom, sizeof(E2PRom_data));
    memcpy(&appInfo->config.dev_count, &ERom.config.dev_count,\
                                   sizeof(ERom.config.dev_count) );
    for(int i=0;i<appInfo->config.dev_count; i++){
        appInfo->devs[i].sens_desc.name_index        = ERom.devs[i].name_index;
        appInfo->devs[i].sens_desc.inst_sized        = (ERom.devs[i].inst_and_addr & 0b10000000) >> 7;
        appInfo->devs[i].sens_desc.address           = ERom.devs[i].inst_and_addr  & 0b01111111;
        appInfo->devs[i].sens_desc.data1.is_signed   = (ERom.devs[i].data_struct & 0b10000000) >> 7;
        appInfo->devs[i].sens_desc.data1.mult_or_div = (ERom.devs[i].data_struct & 0b01000000) >> 6;
        tempFactor                                   = (ERom.devs[i].data_struct & 0b00110000) >> 4;
        appInfo->devs[i].sens_desc.data1.factor      = miniPow(tempFactor);
        appInfo->devs[i].sens_desc.data2.is_signed   = (ERom.devs[i].data_struct & 0b00001000) >> 3;
        appInfo->devs[i].sens_desc.data2.exist       = (ERom.devs[i].data_struct & 0b00000100) >> 2;
        tempFactor                                   = (ERom.devs[i].data_struct & 0b00000011);
        appInfo->devs[i].sens_desc.data2.factor      = miniPow(tempFactor);
    }

    return  HAL_OK;
}


HAL_StatusTypeDef EE_Store(App_info *appInfo){
    E2PRom_data  ERom;
    uint8_t      tempFactor;
    if(EE_checked != 1){
        HAL_StatusTypeDef ret;
        ret = EE_init();
        if(ret != HAL_OK){
            EE_checked = 3;
            _TRAP;
            return ret;
        }
    }
    memset(&ERom,0, sizeof(ERom));
    ERom.config.dev_count = appInfo->config.dev_count;
    for(int i = 0; i < appInfo->config.dev_count;i++){
        ERom.devs[i].inst_and_addr =  appInfo->devs[i].sens_desc.inst_sized << 7;
        ERom.devs[i].inst_and_addr |= appInfo->devs[i].sens_desc.address & 0b01111111;
        ERom.devs[i].name_index    =  appInfo->devs[i].sens_desc.name_index;
        ERom.devs[i].data_struct   =  appInfo->devs[i].sens_desc.data1.is_signed << 7;
        ERom.devs[i].data_struct   |= (appInfo->devs[i].sens_desc.data1.mult_or_div & 0b1) << 6;
        tempFactor                 =  miniLog(appInfo->devs[i].sens_desc.data1.factor);
        ERom.devs[i].data_struct   |= (tempFactor &0b11) << 4;
        ERom.devs[i].data_struct   |= appInfo->devs[i].sens_desc.data2.is_signed << 3;
        ERom.devs[i].data_struct   |= appInfo->devs[i].sens_desc.data2.exist     << 2;
        tempFactor                 =  miniLog(appInfo->devs[i].sens_desc.data2.factor);
        ERom.devs[i].data_struct   |= tempFactor &0b11;
    }
    EE_write(0,(uint8_t*)&ERom,sizeof(ERom));
    return HAL_OK;
}


uint16_t miniPow(uint8_t f){
    switch (f){
        case 0:
            return 1;
        case 1:
            return  10;
        case 2:
            return 100;
        case 3:
            return 1000;
        default:
            printf("不可能的事情发生了!\n");
            _TRAP;
            return 1;
    }

}
static uint8_t  miniLog (uint16_t f){
    switch (f){
        case 1:
            return 0;
        case 10:
            return  1;
        case 100:
            return 2;
        case 1000:
            return 3;
        default:
            printf("不可能的事情发生了!\n");
            _TRAP;
            return 1;
    }

}