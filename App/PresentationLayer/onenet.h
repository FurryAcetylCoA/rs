//
// Created by 11096 on 2022/11/9.
//

#ifndef RS_ONENET_H
#define RS_ONENET_H

#include "main.h"
#include "datastruct.h"

typedef struct {
    void (*Poll)(void);
    void (*Pollall)(void);
    void (*PollOne)(uint8_t);
}Onenet;

extern Onenet onenet;



#endif //RS_ONENET_H
