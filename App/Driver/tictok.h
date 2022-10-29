//
// Created by 11096 on 2022/10/28.
//

#ifndef RS_TICTOK_H
#define RS_TICTOK_H

#include "main.h"
//滴答中间件，其他组件可以注册它的回调
//可以很方便地在预计时间后运行某一逻辑
typedef struct {
    void      (*tick)   (void);
    void      (*Remove) (uint32_t);
    uint32_t  (*Add)    (void(*)(void),uint32_t,_Bool OneShoot);
    void      (*Init)   (void);
}TicTok;

extern TicTok tictok;
//todo：加入定时器

#endif //RS_TICTOK_H
