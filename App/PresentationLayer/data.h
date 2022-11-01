//
// Created by 11096 on 2022/10/31.
//

#ifndef RS_DATA_H
#define RS_DATA_H

#include "main.h"
#include "App.h"

typedef struct {
    void (*Poll)(uint32_t);
    void (*Pollall)(void);
}S_Data;

extern S_Data s_data;



#endif //RS_DATA_H
