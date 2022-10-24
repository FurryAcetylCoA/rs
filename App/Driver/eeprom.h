#ifndef __EEPROM_H__
#define __EEPROM_H__

#include "main.h"

int  EE_check();
void EE_init ();
void EE_Load();
void EE_Store();
							
#endif

typedef __PACKED_STRUCT{
	uint8_t inst_and_addr;
	uint8_t data_struct;
	uint8_t name_index;
	uint8_t not_used;

}E2PRom_dev;
typedef __PACKED_STRUCT{
	uint32_t   basic_config;
	E2PRom_dev devs [10];
}E2PRom_data;

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
    1B  ����
*/