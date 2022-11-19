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
    ST_Genesis = 0, //��ʼ״̬
    ST_Limbo,      //���󲶻���ʾ
    ST_saint_peter,
    ST_Silver_Key, //�ȴ�����������ʾ
    ST_Earth,      //������ʾ
    ST_Golden_Key, //�ȴ������豸ע��
    ST_Empyrean,   //�豸ע��
}States;

typedef enum{
    ES_Devname =0, //��ʼ״̬
    ES_Conform,
    ES_Programing,
    ES_Yet_Another,  //�Ѿ����һ��ע�ᣬ�Ƿ�Ҫ������豸
    ES_Full //���� �����ټ��豸��
}Empyrean_States;

typedef enum{
    sens_success =0,
    sens_failed_crc,//�п��������߻�Ӵ�����
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
    Sens_data_struct data1; //����������Ҫ��δ���raw����            //��ʵ��name_index�Ͳ���Ҫ�������������ݴ���ʽ��
    Sens_data_struct data2; //Ҫ��C֧�ֿ��ﻯ����������ֱ����ί�о����� //���������ø���
    uint32_t  data1_raw;
    uint32_t  data2_raw;
}Sens_dev_desc;// ������·��������������

typedef struct{
    Sens_data_struct data1;
    Sens_data_struct data2;
    uint8_t inst_sized;
    uint8_t data1_unit[6];
    uint8_t data2_unit[6];
    uint8_t name[32];
    uint8_t data1_display_name[32];
    uint8_t data2_display_name[32]; //��������������
    uint8_t data1_name[32];
    uint8_t data2_name[32];  //���������ڷ���onenet�ģ��Ǳ�ֻ��utf8��ת��ʵ�ֲ�����
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
    Empyrean_States es_state;
    uint8_t es_select;
    uint8_t es_select_changed;
    uint8_t es_programing_step;
}Empyrean_Data;

typedef struct {
    uint8_t current_top; //��ǰ�������devs�е�����
    uint8_t buffer [MAX_DEV_COUNT][27];
}Earth_Data;

typedef struct{
    Basic_config config;
    uint32_t total_dev; //����ǳ�����һ�������˶������豸
    struct{    //��ô��һ�½�����Ϊ�˺�����
        States state;
        Key_data keys;
        uint32_t check_dhcp_callback_tictok_ID; //���������Ҫ�����Լ�
        uint32_t onenet_poll_tictok_ID;         //�뿪earth���õ�

    };
    struct{
        void (*state_go) (States);
        void (*init)     (void)  ;
        void (*state_server)(void);
        void (*on_error) (const char*);
    };
    union {//����״̬��ز������л�ʱ������ //д�����������Ϊ��ʡ�ڴ�
        Empyrean_Data ES;
        Earth_Data    EA;
    }su;//state_union
    App_dev_desc devs[MAX_DEV_COUNT];
}App_info;

extern App_info This;
extern const Dev_desc devDesc[];

#endif //RS_DATASTRUCT_H
