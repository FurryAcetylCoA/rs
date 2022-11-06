//
// Created by 11096 on 2022/10/30.
//

#include "key_services.h"
#include "lcd_gxct.h" //我是真不想让这俩模块耦合在一起，但1）我试了几种办法都无法解决LCD频繁刷新时的闪烁问题  2）我不想把子状态机也做成两段的

void static key_services_golden_key();
void static key_services_silver_key();
void static key_services_empyrean();

/* 按键排列
 * ┌────┐
 * │RST │
 * └────┘
 * ┌────┐
 * │KEY2│
 * ├────┼────┐
 * │KEY1│KEY3│
 * ├────┼────┘
 * │KEY0│
 * └────┘
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
    if(This.keys.U != 0){ //任意键
        This.state_go(ST_Empyrean);
    }
}
void static key_services_silver_key(){
    if(This.keys.U != 0){
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
            LCD_clearLineAll();
        }
    }

}