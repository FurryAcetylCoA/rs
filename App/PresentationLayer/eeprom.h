//
// Created by 11096 on 2022/10/27.
//

#ifndef RS_EEPROM_H
#define RS_EEPROM_H
#include "main.h"
#include "eeprom_drv.h"

typedef struct{
    uint8_t  address;
    uint8_t  inst_sized; //1:需要地址  0:不需要地址
    uint8_t name_index;
    Sens_data_struct data1; //这俩描述了要如何处理raw数据
    Sens_data_struct data2; //要是C支持柯里化函数，这里直接用委托就行了
    uint16_t  data1_raw;
    uint16_t  data2_raw;
}Sens_dev_desc;// 数据链路级传感器描述符

typedef struct{
    Sens_dev_desc   sens_desc;
    int32_t  data1;
    int32_t  data2;
    uint8_t  name[32];
}App_dev_desc; // 应用级传感器描述符

typedef struct {
    uint8_t dev_count;
}Basic_config;

typedef struct{
    Basic_config config;
    App_dev_desc devs[10];
}App_info;

HAL_StatusTypeDef  EE_init ();
HAL_StatusTypeDef  EE_Load(App_info*);
HAL_StatusTypeDef  EE_Store(App_info*);

#endif //RS_EEPROM_H
