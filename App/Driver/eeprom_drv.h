#ifndef __EEPROM_H__
#define __EEPROM_H__

#include "main.h"
#include "datastruct.h"



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
