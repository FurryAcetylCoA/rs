//
// Created by AcetylCoA on 2022/11/12.
//

#ifndef RS_HANZI_H
#define RS_HANZI_H
#include "main.h"
#include "w25q128.h"
#include "alientek.h"

typedef __PACKED_STRUCT{
    u8 fontok; //字库存在标志，0XAA，字库正常；其他，字库不存在
    u32 ugbkaddr; //unigbk 的地址
    u32 ugbksize; //unigbk 的大小
    u32 f12addr; //gbk12 地址
    u32 gbk12size; //gbk12 的大小
    u32 f16addr; //gbk16 地址
    u32 gbk16size; //gbk16 的大小
    u32 f24addr; //gbk24 地址
    u32 gkb24size; //gbk24 的大小
}font_info;

extern font_info ftinfo;

void HanZi_init(void);
void Show_Font(u16 x,u16 y,u8 *font,u8 size,u8 mode);

#endif //RS_HANZI_H
