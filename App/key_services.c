//
// Created by 11096 on 2022/10/30.
//

#include "key_services.h"
#include "lcd_gxct.h" //�����治��������ģ�������һ�𣬵�1�������˼��ְ취���޷����LCDƵ��ˢ��ʱ����˸����  2���Ҳ������״̬��Ҳ�������ε�

void static key_services_golden_key();
void static key_services_silver_key();
void static key_services_empyrean();

/* ��������
 * ������������
 * ��RST ��
 * ������������
 * ������������
 * ��KEY2��
 * �����������੤��������
 * ��KEY1��KEY3��
 * �����������੤��������
 * ��KEY0��
 * ������������
 */

void key_services(){
    switch (This.state) {
        case ST_Golden_Key:
            key_services_golden_key();
            break;
        case ST_Silver_Key:
            key_services_silver_key();
            break;
        case ST_Empyrean:
            key_services_empyrean();
            break;
    }

}

void static key_services_golden_key(){
    if(This.keys.U != 0){ //�����
        This.state_go(ST_Empyrean);
    }
}
void static key_services_silver_key(){
    if(This.keys.key2 != 0){
        This.state_go(ST_Golden_Key);
    }else if(This.keys.key0 != 0){
        This.config.dev_count = 0;
        EE_Store(&This);
        HAL_NVIC_SystemReset();
    }
    else if(This.keys.U != 0){
        This.state_go(ST_Earth);
    }
}

void static key_services_empyrean(){
    if(This.su.ES.es_state == ES_Devname){
        if(This.keys.key2 == 1){//up
            This.su.ES.es_select_changed = 1;
            This.su.ES.es_select +=1;
            if(This.su.ES.es_select >= This.total_dev){
                This.su.ES.es_select = 0;
            }
        }else if (This.keys.key0 == 1){//down
            This.su.ES.es_select_changed = 1;
            if (This.su.ES.es_select == 0){ //no count down anymore
                This.su.ES.es_select = This.total_dev; //tricky
            }
            This.su.ES.es_select -=1;
        }else if (This.keys.key3 == 1){//right
            This.su.ES.es_state = ES_Conform;
            LCD_clearLineAll();
        }
    }else if (This.su.ES.es_state == ES_Conform){
        if (This.keys.key1 == 1) {//cancel
            This.su.ES.es_state = ES_Devname;
            LCD_clearLineAll();
        }else if (This.keys.key3 == 1){//enter
            This.su.ES.es_state = ES_Programing;
            This.su.ES.es_programing_step = 0;
            LCD_clearLineAll();
        }
    }else if (This.su.ES.es_state == ES_Programing){
        if(This.su.ES.es_programing_step >= 2 ){
            if(This.keys.key2 == 1){  //try again
                This.su.ES.es_programing_step = 0;
            }else if(This.keys.key1 == 1){// back
                This.su.ES.es_state = ES_Devname;
            }else if(This.keys.key3 == 1){ // OK
                This.config.dev_count += 1;
                This.su.ES.es_state = ES_Yet_Another;
                LCD_clearLineAll();
                EE_Store(&This);
            }
        }
    }else if (This.su.ES.es_state == ES_Yet_Another){
        if(This.keys.key2 == 1){ //Add another
            This.state_go(ST_Empyrean);
        }else if(This.keys.key3 == 1){ // Exit
            This.state_go(ST_Earth);
        }

    }else if (This.su.ES.es_state == ES_Full){
        if(This.keys.U != 0){ //��Ȼ��full�� ���豸�б��Ȼ������
            This.state_go(ST_Earth);
        }
    }

}