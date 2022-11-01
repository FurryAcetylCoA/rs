//
// Created by 11096 on 2022/10/27.
//

#ifndef RS_EEPROM_H
#define RS_EEPROM_H
#include "main.h"
#include "App.h"

HAL_StatusTypeDef  EE_init ();
HAL_StatusTypeDef  EE_Load(App_info*);
HAL_StatusTypeDef  EE_Store(App_info*);

#endif //RS_EEPROM_H
