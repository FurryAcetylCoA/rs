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
//code �ַ�ָ�뿪ʼ
//���ֿ��в��ҳ���ģ
//code �ַ����Ŀ�ʼ��ַ,GBK ��
//mat ���ݴ�ŵ�ַ (size/8+((size%8)?1:0))*(size) bytes ��С
//size:�����С
static void Get_HzMat(unsigned char *code,unsigned char *mat,u8 size)
{
    unsigned char qh,ql;
    unsigned char i;
    unsigned long foffset;
    u8 csize=(size/8+((size%8)?1:0))*(size);//�õ�����һ���ַ���Ӧ������ռ���ֽ���
    qh=*code;
    ql=*(++code);
    if(qh<0x81||ql<0x40||ql==0xff||qh==0xff)//�� ���ú���
    {
        for(i=0;i<csize;i++)*mat++=0x00;//�������
        return; //��������
    }
    if(ql<0x7f)ql-=0x40;//ע��!
    else ql-=0x41;
    qh-=0x81;
    foffset=((unsigned long)190*qh+ql)*csize; //�õ��ֿ��е��ֽ�ƫ����
    switch(size)
    {
        case 12:w25q128_read_data(foffset+ftinfo.f12addr,mat,csize); break;
        case 16:w25q128_read_data(foffset+ftinfo.f16addr,mat,csize);break;
        case 24:w25q128_read_data(foffset+ftinfo.f24addr,mat,csize);break;
        default:break;
    }
}

//��ʾһ��ָ����С�ĺ���
//x,y :���ֵ�����
//font:���� GBK ��
//size:�����С
//mode:0,������ʾ,1,������ʾ
void Show_Font(u16 x,u16 y,u8 *font,u8 size,u8 mode)
{
    u8 temp,t,t1;
    u16 y0=y;
    u8 dzk[72];
    u8 csize=(size/8+((size%8)?1:0))*(size);//�õ�����һ���ַ���Ӧ������ռ���ֽ���
    if(size!=12&&size!=16&&size!=24)return; //��֧�ֵ� size
    Get_HzMat(font,dzk,size); //�õ���Ӧ��С�ĵ�������
    for(t=0;t<csize;t++)
    {
        temp=dzk[t]; //�õ���������
        for(t1=0;t1<8;t1++)
        {
            if(temp&0x80)LCD_Fast_DrawPoint(x,y,POINT_COLOR);
            else if(mode==0)LCD_Fast_DrawPoint(x,y,BACK_COLOR);
            temp<<=1;y++;
            if((y-y0)==size) { y=y0; x++; break;}
        }
    }
}