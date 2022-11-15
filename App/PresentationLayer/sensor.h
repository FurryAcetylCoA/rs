//
// Created by 11096 on 2022/10/29.
//

#ifndef RS_SENSOR_H
#define RS_SENSOR_H
#include "main.h"
#include "App.h"



sens_ErrCode sens_SetAddr(Sens_dev_desc *dev,uint8_t addr);
sens_ErrCode sens_GetVal (Sens_dev_desc *dev);

#endif //RS_SENSOR_H
