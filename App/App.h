//
// Created by 11096 on 2022/10/26.
//

#ifndef RS_APP_H
#define RS_APP_H
#include "main.h"
#include "eeprom.h"
#include "key.h"

/////////////////////////////////
//            ENUM             //
/////////////////////////////////
typedef enum{
    ST_Genesis =0, //初始状态
    ST_saint_peter,
    ST_Silver_Key, //等待进入正常显示
    ST_Earth,      //正常显示
    ST_Golden_Key, //等待进入设备注册
    ST_Empyrean,   //设备注册
}States;

/////////////////////////////////
//           STRUCT            //
/////////////////////////////////
typedef struct{
    uint8_t  address;
    uint8_t  inst_sized; //1:需要地址  0:不需要地址
    uint8_t name_index;
    Sens_data_struct data1; //这俩描述了要如何处理raw数据
    Sens_data_struct data2; //要是C支持柯里化函数，这里直接用委托就行了
    uint16_t  data1_raw;
    uint16_t  data2_raw;
}Sens_dev_desc;// 数据链路级传感器描述符

typedef struct{
    Sens_dev_desc   sens_desc;
    int32_t  data1;
    int32_t  data2;
    uint8_t  name[32];
}App_dev_desc; // 应用级传感器描述符

typedef struct {
    uint8_t eeprom_ready;
    uint8_t dev_count;
}Basic_config;


typedef struct{
    Basic_config config;
    struct{    //这么包一下仅仅是为了好区分
        States state;
        Key_data keys;
    };
    struct{
        void (*state_go) (States);
        void (*init)     (void)  ;
        void (*state_server)(void);
    };
    App_dev_desc devs[10];
}App_info;

extern App_info This;

////////////////////
//    FUNCTION    //
////////////////////

void   App_test_LD_STORE();
#endif //RS_APP_H
