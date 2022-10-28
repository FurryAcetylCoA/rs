#ifndef __EEPROM_H__
#define __EEPROM_H__

#include "main.h"

typedef __PACKED_STRUCT{
    uint8_t inst_and_addr;
    uint8_t data_struct;
    uint8_t name_index;
   // uint8_t not_used;

}E2PRom_dev;

typedef __PACKED_STRUCT{
    uint8_t dev_count;
   // uint8_t dummy1;
    //uint8_t dummy2;
    //uint8_t dummy3;
}E2PRom_config;

typedef __PACKED_STRUCT{
    E2PRom_config  config;
    E2PRom_dev devs [10];
}E2PRom_data;

typedef struct {
    uint8_t   is_signed;
    union {  //����data1.�ǳ˳��жϡ�����data2�Ǵ������жϡ�data2ͳһΪ��
        uint8_t mult_or_div;
        uint8_t exist;
    };
    uint16_t  factor; //ԭֵ������log���
}Sens_data_struct;

typedef struct{
    uint8_t  address;
    uint8_t  inst_sized; //1:��Ҫ��ַ  0:����Ҫ��ַ
    uint8_t name_index;
    Sens_data_struct data1;
    Sens_data_struct data2; //Ҫ��C֧�ֿ��ﻯ����������ֱ����ί�о�����
}Sens_dev_desc;// ������·��������������

typedef struct{
    Sens_dev_desc   sens_desc;
    int32_t  data1;
    int32_t  data2;
    uint8_t  name[32];
}App_dev_desc; // Ӧ�ü�������������

typedef struct {
    uint8_t dev_count;
}Basic_config;

typedef struct{
    Basic_config config;
    App_dev_desc devs[10];
}App_info; //todo:move this

/* ��32ҳ
struct
  .0B
	һҳ  �豸��������������
  .1B
	4B  devs  //�����10��
  ...
  .42B

struct dev
	1B  ָ���ʽ[7]:��ַ[6:0]
	1B  ����1[7]:�˳�1[6]:ϵ��log10-1[5:4]:����2[3]:����[2]:����log10-2[1:0]
    1B  ���ƴ���
*/


void  EE_write(uint8_t addr,uint8_t *data,uint32_t len);
void  EE_read (uint8_t addr,uint8_t *data,uint32_t len);


#endif
