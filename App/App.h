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
    ST_Genesis =0, //��ʼ״̬
    ST_saint_peter,
    ST_Silver_Key, //�ȴ�����������ʾ
    ST_Earth,      //������ʾ
    ST_Golden_Key, //�ȴ������豸ע��
    ST_Empyrean,   //�豸ע��
}States;

/////////////////////////////////
//           STRUCT            //
/////////////////////////////////
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
    };
    App_dev_desc devs[10];
}App_info;

extern App_info This;

////////////////////
//    FUNCTION    //
////////////////////

void   App_test_LD_STORE();
#endif //RS_APP_H
