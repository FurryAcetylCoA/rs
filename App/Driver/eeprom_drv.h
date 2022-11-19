#ifndef __EEPROM_H__
#define __EEPROM_H__

#include "main.h"
#include "datastruct.h"



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
*/


void  EE_write(uint8_t addr,uint8_t *data,uint32_t len);
void  EE_read (uint8_t addr,uint8_t *data,uint32_t len);


#endif
