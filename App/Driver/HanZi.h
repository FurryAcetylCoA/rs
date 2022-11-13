//
// Created by AcetylCoA on 2022/11/12.
//

#ifndef RS_HANZI_H
#define RS_HANZI_H
#include "main.h"
#include "w25q128.h"
#include "alientek.h"

typedef __PACKED_STRUCT{
    u8 fontok; //�ֿ���ڱ�־��0XAA���ֿ��������������ֿⲻ����
    u32 ugbkaddr; //unigbk �ĵ�ַ
    u32 ugbksize; //unigbk �Ĵ�С
    u32 f12addr; //gbk12 ��ַ
    u32 gbk12size; //gbk12 �Ĵ�С
    u32 f16addr; //gbk16 ��ַ
    u32 gbk16size; //gbk16 �Ĵ�С
    u32 f24addr; //gbk24 ��ַ
    u32 gkb24size; //gbk24 �Ĵ�С
}font_info;

extern font_info ftinfo;

void HanZi_init(void);
void Show_Font(u16 x,u16 y,u8 *font,u8 size,u8 mode);

#endif //RS_HANZI_H
