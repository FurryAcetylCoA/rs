//
// Created by AcetylCoA on 2022/11/12.
//

#include "HanZi.h"
#include "lcd.h"

#define  FONTINFOADDR (1024*1024*12)

font_info ftinfo;

static void Get_HzMat(unsigned char *code,unsigned char *mat,u8 size);

void HanZi_init(void){

    w25q128_read_data(FONTINFOADDR, (uint8_t *) &ftinfo, sizeof(ftinfo));

    if(ftinfo.fontok != 0xAA){
        This.on_error(__func__);
    }

}
//code 字符指针开始
//从字库中查找出字模
//code 字符串的开始地址,GBK 码
//mat 数据存放地址 (size/8+((size%8)?1:0))*(size) bytes 大小
//size:字体大小
static void Get_HzMat(unsigned char *code,unsigned char *mat,u8 size)
{
    unsigned char qh,ql;
    unsigned char i;
    unsigned long foffset;
    u8 csize=(size/8+((size%8)?1:0))*(size);//得到字体一个字符对应点阵集所占的字节数
    qh=*code;
    ql=*(++code);
    if(qh<0x81||ql<0x40||ql==0xff||qh==0xff)//非 常用汉字
    {
        for(i=0;i<csize;i++)*mat++=0x00;//填充满格
        return; //结束访问
    }
    if(ql<0x7f)ql-=0x40;//注意!
    else ql-=0x41;
    qh-=0x81;
    foffset=((unsigned long)190*qh+ql)*csize; //得到字库中的字节偏移量
    switch(size)
    {
        case 12:w25q128_read_data(foffset+ftinfo.f12addr,mat,csize); break;
        case 16:w25q128_read_data(foffset+ftinfo.f16addr,mat,csize);break;
        case 24:w25q128_read_data(foffset+ftinfo.f24addr,mat,csize);break;
        default:break;
    }
}

//显示一个指定大小的汉字
//x,y :汉字的坐标
//font:汉字 GBK 码
//size:字体大小
//mode:0,正常显示,1,叠加显示
void Show_Font(u16 x,u16 y,u8 *font,u8 size,u8 mode)
{
    u8 temp,t,t1;
    u16 y0=y;
    u8 dzk[72];
    u8 csize=(size/8+((size%8)?1:0))*(size);//得到字体一个字符对应点阵集所占的字节数
    if(size!=12&&size!=16&&size!=24)return; //不支持的 size
    Get_HzMat(font,dzk,size); //得到相应大小的点阵数据
    for(t=0;t<csize;t++)
    {
        temp=dzk[t]; //得到点阵数据
        for(t1=0;t1<8;t1++)
        {
            if(temp&0x80)LCD_Fast_DrawPoint(x,y,POINT_COLOR);
            else if(mode==0)LCD_Fast_DrawPoint(x,y,BACK_COLOR);
            temp<<=1;y++;
            if((y-y0)==size) { y=y0; x++; break;}
        }
    }
}

wchar_t ff_convert ( /* Converted code, 0 means conversion error */
        wchar_t src,/* Character code to be converted */
        uint32_t dir/* 0: Unicode to OEMCP, 1: OEMCP to Unicode */
        )
{
    wchar_t t[2];
    wchar_t c;
    u32 i, li, hi;
    u16 n;
    u32 gbk2uni_offset=0;
    if (src < 0x80)c = src;//ASCII,直接不用转换.
    else
    {
        if(dir) gbk2uni_offset=ftinfo.ugbksize/2;    //GBK 2 UNICODE
        else gbk2uni_offset=0;                       //UNICODE 2 GBK
        /* Unicode to OEMCP */
        hi=ftinfo.ugbksize/2;//对半开.
        hi =hi / 4 - 1;
        li = 0;
        for (n = 16; n; n--){
            i = li + (hi - li) / 2;
            w25q128_read_data(ftinfo.ugbkaddr+i*4+gbk2uni_offset,(u8*)&t,4);//读出 4 个字节
            if (src == t[0]) break;
            if (src > t[0])li = i;
            else hi = i;
        }
        c = n ? t[1]:0;
    }
    return c;
}