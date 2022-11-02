//
// Created by 11096 on 2022/11/1.
//

#ifndef RS_DATASTRUCT_H
#define RS_DATASTRUCT_H
#include "stm32f4xx_hal.h"

/////////////////////////////////
//            ENUM             //
/////////////////////////////////
typedef enum{
    ST_Genesis =0, //��ʼ״̬
    ST_Limbo,      //���󲶻���ʾ
    ST_saint_peter,
    ST_Silver_Key, //�ȴ�����������ʾ
    ST_Earth,      //������ʾ
    ST_Golden_Key, //�ȴ������豸ע��
    ST_Empyrean,   //�豸ע��
}States;

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
            uint8_t dataLen;  //�����Ͳ����
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
    Sens_data_struct data1; //����������Ҫ��δ���raw����
    Sens_data_struct data2; //Ҫ��C֧�ֿ��ﻯ����������ֱ����ί�о�����
    uint16_t  data1_raw;
    uint16_t  data2_raw;
}Sens_dev_desc;// ������·��������������

typedef struct{
    Sens_dev_desc   sens_desc;
    int32_t  data1;
    int32_t  data2;
    uint8_t  name[32];
}App_dev_desc; // Ӧ�ü�������������

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
    Basic_config config;
    struct{    //��ô��һ�½�����Ϊ�˺�����
        States state;
        Key_data keys;
    };
    struct{
        void (*state_go) (States);
        void (*init)     (void)  ;
        void (*state_server)(void);
        void (*on_error) (const char*);
    };
    App_dev_desc devs[10];
}App_info;

extern App_info This;

#endif //RS_DATASTRUCT_H
