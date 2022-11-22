//
// Created by 11096 on 2022/10/26.
//

#include "App.h"
#include "string.h"
#include "stdio.h"
#include "tictok.h"
#include "data.h"
#include "lcd_gxct.h"
#include "sensor.h"
#include "lcd_server.h"
#include "HanZi.h"
#include "onenet.h"

//����֧�ֵĴ�����
const Dev_desc devDesc[]={                  //Ϊ�˷����ֿ�Ĳ�������˫���Ŵ����
        {.name="Air Temp&Humidity",.inst_sized=0, .data2.exist=1,
        .data1_display_name="�¶�" , .data2_display_name="ʪ��",.data1_name="Temperature",.data2_name="Humidity",
        .data1_unit="\"C"  ,.data1.factor=10, .data1.is_signed=1,.data1.mult_or_div=1,
        .data2_unit="%RH",  .data2.factor=10, .data2.is_signed=1 },

        {.name="CO2"              ,.inst_sized=0 ,.data2.exist=0,
        .data1_display_name="������̼Ũ��",.data1_name="",
        .data1_unit="ppm"  ,.data1.factor=1  ,.data1.is_signed=0,.data1.mult_or_div=1},

        {.name="Soil Conductance" ,.inst_sized=0,.data2.exist=0,
        .data1_display_name="�����絼��",.data1_name="",
        .data1_unit="mS/cm",.data1.factor=100,.data1.is_signed=1,.data1.mult_or_div=1},

        {.name="PH"            ,.inst_sized=1,.data2.exist=0,
        .data1_display_name="PHֵ" ,.data1_name="",
        .data1_unit="pH",.data1.factor=100,.data1.is_signed=0,.data1.mult_or_div=1 },

        {.name="Illuminance" ,.inst_sized=0,.data2.exist=0,
         .data1_display_name="��ǿ",.data1_name="",
         .data1_unit="Lux",.data1.factor=10,.data1.is_signed=0,.data1.mult_or_div=0}

};//��ֻ���������´������������Ӱ�����д�����



static void ST_Empyrean_Program_dev();
static void App_init();
static void State_go(States next_state);
static void State_server(void);
static void on_error (const char*);



App_info This={
        .init     = App_init,
        .state_go = State_go,
        .state_server = State_server,
        .on_error = on_error
};


static void App_init(){
    This.total_dev=sizeof(devDesc)/sizeof (devDesc[0]);//����Ҫ������ļ���ȡ����Ϊsiezof�Ǳ�����ȷ������extern ��������ȷ��
    tictok.Init();
    LCD_Init(GRAYBLUE);
    HanZi_init();

    This.state_go(ST_Genesis);
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
            }break;
        case ST_Empyrean:
            if (This.su.ES.es_state == ES_Programing){
                ST_Empyrean_Program_dev();
            }
            break;
        default:
            break;
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
    if(This.state == ST_Limbo){
        return; //limbo ״̬�������Ƴ�
    }
    lcd_state_go(next_state);

    switch (next_state) {
        case ST_Genesis:
            This.state=ST_Genesis;
            break;
        case ST_Silver_Key:
            EE_Load(&This);
            This.state=ST_Silver_Key;
            LCD_clearLineAll();
            break;
        case ST_saint_peter:
            EE_Load(&This);//����EEPROM����
            This.state=ST_saint_peter;
            break;
        case ST_Earth:
            //����������ݣ�Ȼ��ע�������м����ѯ��ʱ������
            //onenet�¼�ע��������
            //����������뿪earth����ǵ�Ҫע��onenet
            memset(&This.su,0,sizeof(This.su));

            s_data.Pollall();
            if(This.config.dev_count <=2) {
                tictok.Add(s_data.Poll, 1000, false);
            }else{
                tictok.Add(s_data.Poll,300,false);
            }

            This.onenet_poll_tictok_ID=tictok.Add(onenet.Poll,10000,false);
            onenet.Pollall();

            This.state=ST_Earth;
            LCD_clearLineAll();
            break;
        case ST_Golden_Key:
            //�����豸ע��
            This.state=ST_Golden_Key;
            break;
        case ST_Empyrean:
            EE_Load(&This);
            memset(&This.su, 0, sizeof(This.su));
            if(This.config.dev_count>=lenof(This.devs)){
                This.su.ES.es_state = ES_Full;
            }
            This.state=ST_Empyrean;
            break;
        case ST_Limbo:
            tictok.Wipe();
            This.state=ST_Limbo;
            break;
        default:
            _TRAP;

    }
}

//��ע�Ṧ����صĹ���
//���ݵ�ǰ��ѡ�����һ���豸������
//Ȼ����ݵ�ǰ����������һ���У�
//   0��������������sens_SetAddr����
//   1������ͨ��s_data.PollOne��ȡ
//   2����ȡ�ɹ� ����> �޸�dev_count & д��EEPROM
//���Ʋ������ı�����es_programing_step��
static void ST_Empyrean_Program_dev(){
    App_dev_desc   *ndev = &This.devs[This.config.dev_count];
    switch (This.su.ES.es_programing_step) {
        case 0:{

            Dev_desc const *ddev = &devDesc[This.su.ES.es_select];
            ndev->sens_desc.name_index = This.su.ES.es_select;
            ndev->sens_desc.inst_sized = ddev->inst_sized;
            //memcpy(ndev->name,ddev->name, strlen((const char*)ddev->name));
            ndev->sens_desc.data1.factor      = ddev->data1.factor;
            ndev->sens_desc.data1.is_signed   = ddev->data1.is_signed;
            ndev->sens_desc.data1.mult_or_div = ddev->data1.mult_or_div;
            if (ddev->data2.exist == 1){
                ndev->sens_desc.data2.exist     = 1;
                ndev->sens_desc.data2.factor    = ddev->data2.factor;
                ndev->sens_desc.data2.is_signed = ddev->data2.is_signed;
            }
            sens_SetAddr(&ndev->sens_desc,This.config.dev_count+1);
        }   This.su.ES.es_programing_step++;
            break;
        case 1:
            s_data.PollOne(This.config.dev_count +1 -1);
            This.su.ES.es_programing_step++;
            break;
        case 2:
            //nothing
            break;
        default:
            break;

    }
}



static void on_error (const char* err){
    This.state_go(ST_Limbo);
    LCD_push(RED);
    char buffer[26];
    snprintf((char*)buffer,25,"ERR:%s     ",err);
    LCD_ShowStringLine(LINE10, buffer);
    LCD_pop();
}
