//
// Created by 11096 on 2022/10/28.
//

#ifndef RS_TICTOK_H
#define RS_TICTOK_H

#include "main.h"
//�δ��м���������������ע�����Ļص�
//���Ժܷ������Ԥ��ʱ�������ĳһ�߼�
typedef struct {
    void      (*tick)   (void);
    void      (*Remove) (uint32_t);
    uint32_t  (*Add)    (void(*)(void),uint32_t,_Bool OneShoot);
    void      (*Init)   (void);
}TicTok;

extern TicTok tictok;
//todo�����붨ʱ��

#endif //RS_TICTOK_H
