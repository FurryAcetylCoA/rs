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
    union {  //对于data1.是乘除判断。对于data2是存在性判断。data2统一为除
        uint8_t mult_or_div;
        uint8_t exist;
    };
    uint16_t  factor; //原值。不是log后的
}Sens_data_struct;

typedef struct{
    uint8_t  address;
    uint8_t  inst_sized; //1:需要地址  0:不需要地址
    uint8_t name_index;
    Sens_data_struct data1;
    Sens_data_struct data2; //要是C支持柯里化函数，这里直接用委托就行了
}Sens_dev_desc;// 数据链路级传感器描述符

typedef struct{
    Sens_dev_desc   sens_desc;
    int32_t  data1;
    int32_t  data2;
    uint8_t  name[32];
}App_dev_desc; // 应用级传感器描述符

typedef struct {
    uint8_t dev_count;
}Basic_config;

typedef struct{
    Basic_config config;
    App_dev_desc devs[10];
}App_info; //todo:move this

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
