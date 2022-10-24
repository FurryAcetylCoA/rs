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

/* 共32页
struct 
  .0B	
	一页  设备总数：其他配置
  .1B
	4B  devs  //最多是10个
  ...
  .42B
			
struct dev 
	1B  指令格式[7]:地址[6:0]
	1B  符号1[7]:乘除1[6]:系数log10-1[5:4]:符号2[3]:有无[2]:除数log10-2[1:0]
    1B  名称代码
    1B  备用
*/