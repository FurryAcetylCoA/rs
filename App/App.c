//
// Created by 11096 on 2022/10/26.
//

#include "App.h"
#include "string.h"
#include "stdio.h"
#include "tictok.h"
#include "data.h"

static void App_init();
static void State_go(States next_state);
static void State_server(void);
App_info This={
        .init     = App_init,
        .state_go = State_go,
        .state_server = State_server
};

void App_test_LD_STORE(){

    This.config.dev_count = 3;
    This.devs[0].sens_desc.address = 0x32;
    This.devs[0].sens_desc.inst_sized = 1;
    This.devs[0].sens_desc.name_index = 3;
    This.devs[0].sens_desc.data1.is_signed = 1;
    This.devs[0].sens_desc.data1.factor    = 1000;
    This.devs[0].sens_desc.data1.mult_or_div = 1;
    This.devs[0].sens_desc.data2.exist =1;
    This.devs[0].sens_desc.data2.factor = 10;
    This.devs[1].sens_desc.address = 0x77;
    This.devs[1].sens_desc.inst_sized = 0;
    This.devs[1].sens_desc.name_index = 7;
    This.devs[1].sens_desc.data1.is_signed = 0;
    This.devs[1].sens_desc.data1.factor = 100;
    This.devs[1].sens_desc.data1.mult_or_div = 0;
    This.devs[1].sens_desc.data2.exist=0;
    This.devs[2].sens_desc.address = 0x01;
    This.devs[2].sens_desc.inst_sized = 0;
    This.devs[2].sens_desc.name_index = 9;
    This.devs[2].sens_desc.data1.is_signed = 0;
    This.devs[2].sens_desc.data1.factor = 1;
    This.devs[2].sens_desc.data1.mult_or_div = 0;
    This.devs[2].sens_desc.data2.exist=1;
    This.devs[2].sens_desc.data2.is_signed=1;
    This.devs[2].sens_desc.data2.factor = 1000;
    HAL_StatusTypeDef e;
    e = EE_Store(&This);
    App_info This2; //Ӧ�ý����
    //sizeof "This" is 524
    if(e!=HAL_OK){
        _TRAP;
    }
    memset(&This2,0,sizeof(This2));
    EE_Load(&This2);

    if(memcmp(This.devs,This2.devs,sizeof(This.devs))!=0){
        _TRAP;
    }
    _TRAP;
}
static void App_init(){

}

/**
* @brief ״̬��������߼����������ĳ״̬ʱ���������Ĳ�����
* �Զ�ת�ƻ�д������
* @retval None.
*/
static void State_server(){
    switch (This.state) {
        case ST_saint_peter:
            //eeprom��ȡ�ɹ�+���豸ע��->silver_key
            if(This.config.eeprom_ready == 1){
                if(This.config.dev_count != 0 ){
                    State_go(ST_Silver_Key);
                }else{
                    State_go(ST_Golden_Key);
                }
            }

    }
}
/**
* @brief ����This״̬��ת��״̬
* һ���Բ�������������
* @param next_state: next state
* @retval None.
*/
static void State_go(States next_state){
    //״̬ת��
    switch (next_state) {
        case ST_Genesis:
            _TRAP;
            break;
        case ST_saint_peter:
            EE_Load(&This);//����EEPROM����
            This.state=ST_saint_peter;
            break;
        case ST_Earth:
            //����������ݣ�Ȼ��ע�������м����ѯ��ʱ������
            s_data.Pollall();
            tictok.Add(s_data.Poll,1000,false);
            This.state=ST_Earth;
            break;
        case ST_Golden_Key:
            //�����豸ע��
            This.state=ST_Golden_Key;
            break;
        case ST_Empyrean:
            EE_Load(&This);
            This.state=ST_Empyrean;
            break;

        default:
            _TRAP;

    }
}
