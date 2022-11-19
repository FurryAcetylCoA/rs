//
// Created by 11096 on 2022/11/1.
//

#ifndef RS_DATASTRUCT_H
#define RS_DATASTRUCT_H

#include "stm32f4xx_hal.h"
#include "http_client_x.h"

/////////////////////////////////
//       CONST    DEFINE       //
/////////////////////////////////

#define MAX_DEV_COUNT 5


/////////////////////////////////
//            ENUM             //
/////////////////////////////////
typedef enum{
    ST_Genesis = 0, //初始状态
    ST_Limbo,      //错误捕获及提示
    ST_saint_peter,
    ST_Silver_Key, //等待进入正常显示
    ST_Earth,      //正常显示
    ST_Golden_Key, //等待进入设备注册
    ST_Empyrean,   //设备注册
}States;

typedef enum{
    ES_Devname =0, //初始状态
    ES_Conform,
    ES_Programing,
    ES_Yet_Another,  //已经完成一轮注册，是否要再添加设备
    ES_Full //满了 不能再加设备了
}Empyrean_States;

typedef enum{
    sens_success =0,
    sens_failed_crc,//有可能是离线或接触不良
    sens_failed_other
}sens_ErrCode;

/////////////////////////////////
//           STRUCT            //
/////////////////////////////////

typedef __PACKED_STRUCT {
    uint8_t address;
    uint8_t opCode;
    __PACKED_UNION{
        __PACKED_STRUCT{
            uint8_t dataAddHi;
            uint8_t dataAddLo;
            uint8_t dataHi;
            uint8_t dataLo;
            uint8_t crc16Lo;
            uint8_t crc16Hi;
        }withNoLen;
        __PACKED_STRUCT{
            uint8_t dataLen;  //这俩就差这块
            uint8_t dataAddHi;
            uint8_t dataAddLo;
            uint8_t dataHi;
            uint8_t dataLo;
            uint8_t crc16Lo;
            uint8_t crc16Hi;
        }withLen;
    };
}Sens_buffer;

typedef __PACKED_STRUCT{
    uint8_t inst_and_addr;
    uint8_t data_struct;
    uint8_t name_index;

}E2PRom_dev;

typedef __PACKED_STRUCT{
    uint8_t dev_count;
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
    Sens_data_struct data1; //这俩描述了要如何处理raw数据            //其实有name_index就不需要这俩来保存数据处理方式了
    Sens_data_struct data2; //要是C支持柯里化函数，这里直接用委托就行了 //但是我懒得改了
    uint32_t  data1_raw;
    uint32_t  data2_raw;
}Sens_dev_desc;// 数据链路级传感器描述符

typedef struct{
    Sens_data_struct data1;
    Sens_data_struct data2;
    uint8_t inst_sized;
    uint8_t data1_unit[6];
    uint8_t data2_unit[6];
    uint8_t name[32];
    uint8_t data1_display_name[32];
    uint8_t data2_display_name[32]; //这俩可以是中文
    uint8_t data1_name[32];
    uint8_t data2_name[32];  //这俩是用于发给onenet的，那边只认utf8，转码实现不出来
}Dev_desc;

typedef struct {
    httpc_connection_t httpc_settings;
    _Bool  sending;
}Dev_onenet_desc;

typedef struct{
    Sens_dev_desc   sens_desc;
    float  data1;
    float  data2;
    sens_ErrCode  errCode;
    Dev_onenet_desc onenetDesc;
}App_dev_desc; // 应用级传感器描述符

typedef struct {
    uint8_t eeprom_ready;
    uint8_t dev_count;
}Basic_config;

typedef __PACKED_UNION {
    __PACKED_STRUCT{
        uint8_t  key0;
        uint8_t  key1;
        uint8_t  key2;
        uint8_t  key3;
    };
    uint32_t U;
}Key_data;

typedef struct{
    Empyrean_States es_state;
    uint8_t es_select;
    uint8_t es_select_changed;
    uint8_t es_programing_step;
}Empyrean_Data;

typedef struct {
    uint8_t current_top; //当前最高行在devs中的索引
    uint8_t buffer [MAX_DEV_COUNT][27];
}Earth_Data;

typedef struct{
    Basic_config config;
    uint32_t total_dev; //这个是程序里一共描述了多少种设备
    struct{    //这么包一下仅仅是为了好区分
        States state;
        Key_data keys;
        uint32_t check_dhcp_callback_tictok_ID; //这个任务需要结束自己
        uint32_t onenet_poll_tictok_ID;         //离开earth会用到

    };
    struct{
        void (*state_go) (States);
        void (*init)     (void)  ;
        void (*state_server)(void);
        void (*on_error) (const char*);
    };
    union {//运行状态相关参数，切换时不保留 //写成联合体仅仅为了省内存
        Empyrean_Data ES;
        Earth_Data    EA;
    }su;//state_union
    App_dev_desc devs[MAX_DEV_COUNT];
}App_info;

extern App_info This;
extern const Dev_desc devDesc[];

#endif //RS_DATASTRUCT_H
